#include <kernel/int.h>
#include <kernel/terminal.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000
#define SCROLLBACK_LINES 200

static volatile u16 *vga = (u16 *)VGA_MEMORY;
static u16 buffer[SCROLLBACK_LINES][VGA_WIDTH];

static int write_row = 0; // logical row index — grows forever, never resets
static int write_col = 0;
static int view_offset =
    0; // 0 = live/bottom; N = scrolled back N lines into history
static u8 line_len[SCROLLBACK_LINES]; // how many columns are actually used in
                                      // each row

static u8 term_color = 0x0F;

static inline void outb(u16 port, u8 val) {
  __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline u16 vga_entry(char c, u8 color) {
  return (u16)c | ((u16)color << 8);
}

static u16 *buf_row(int row) {
  return buffer[row % SCROLLBACK_LINES]; // ring buffer — old lines get
                                         // overwritten past 200
}

static void clear_row(int row) {
  u16 *r = buf_row(row);
  for (int c = 0; c < VGA_WIDTH; c++)
    r[c] = vga_entry(' ', term_color);
  line_len[row % SCROLLBACK_LINES] = 0;
}

static int max_scroll_offset(void) {
  int earliest = (write_row + 1 > SCROLLBACK_LINES)
                     ? (write_row + 1 - SCROLLBACK_LINES)
                     : 0;
  int max_off = (write_row - (VGA_HEIGHT - 1)) - earliest;
  return max_off > 0 ? max_off : 0;
}

static void hide_cursor(void) {
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20); // bit 5 set = cursor disabled
}

static void show_cursor(void) {
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x00); // start scanline 0, bit 5 clear = cursor enabled (default
                     // underline shape)
}

static void update_cursor(void) {
  if (view_offset != 0) {
    hide_cursor();
    return;
  }
  show_cursor();

  int start = write_row - (VGA_HEIGHT - 1);
  if (start < 0)
    start = 0;

  int screen_row = write_row - start;
  u16 pos = screen_row * VGA_WIDTH + write_col;
  outb(0x3D4, 14);
  outb(0x3D5, (u8)(pos >> 8));
  outb(0x3D4, 15);
  outb(0x3D5, (u8)(pos & 0xFF));
}

static void render_viewport(void) {
  int start = write_row - (VGA_HEIGHT - 1) - view_offset;
  if (start < 0)
    start = 0;

  for (int r = 0; r < VGA_HEIGHT; r++) {
    int src = start + r;
    u16 *dst = (u16 *)&vga[r * VGA_WIDTH];
    if (src <= write_row) {
      u16 *s = buf_row(src);
      for (int c = 0; c < VGA_WIDTH; c++)
        dst[c] = s[c];
    } else {
      for (int c = 0; c < VGA_WIDTH; c++)
        dst[c] = vga_entry(' ', term_color);
    }
  }
}

static void newline(void) {
  write_row++;
  write_col = 0;
  clear_row(
      write_row); // clears stale content if this ring slot is being reused
}

void terminal_initialize(void) {
  for (int i = 0; i < SCROLLBACK_LINES; i++)
    clear_row(i);
  write_row = 0;
  write_col = 0;
  view_offset = 0;
  render_viewport();
  update_cursor();
}

void terminal_putchar(char c) {
  view_offset = 0; // typing always snaps the view back to live — standard
                   // terminal behavior

  if (c == '\n') {
    newline();
  } else if (c == '\b') {
    if (write_col > 0) {
      write_col--;
      buf_row(write_row)[write_col] = vga_entry(' ', term_color);
      line_len[write_row % SCROLLBACK_LINES] = write_col;
    } else if (write_row > 0) {
      write_row--;
      write_col = line_len[write_row % SCROLLBACK_LINES];
      // Just move the cursor to just past the previous line's last
      // character — don't erase anything yet. The *next* backspace
      // (write_col > 0 now) will delete it normally.
    }
  } else {
    buf_row(write_row)[write_col] = vga_entry(c, term_color);
    write_col++;
    line_len[write_row % SCROLLBACK_LINES] = write_col;
    if (write_col >= VGA_WIDTH) {
      newline();
    }
  }

  render_viewport();
  update_cursor();
}

void terminal_writestring(const char *str) {
  for (int i = 0; str[i] != '\0'; i++)
    terminal_putchar(str[i]);
}

void terminal_scroll(int delta) {
  view_offset += delta;
  if (view_offset < 0)
    view_offset = 0;
  int max_off = max_scroll_offset();
  if (view_offset > max_off)
    view_offset = max_off;

  render_viewport();
  update_cursor();
}

void terminal_set_cursor_shape(u8 start_scanline, u8 end_scanline) {
  outb(0x3D4, 0x0A);
  outb(0x3D5, start_scanline & 0x1F);
  outb(0x3D4, 0x0B);
  outb(0x3D5, end_scanline & 0x1F);
}
