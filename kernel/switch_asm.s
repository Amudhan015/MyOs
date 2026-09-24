.global switch_task
switch_task:
    pushfl            // save EFLAGS (including IF) along with the registers
    push %ebp
    push %ebx
    push %esi
    push %edi

    mov 24(%esp), %eax   // old_task_esp_ptr — offset grew by 4 (pushfd added)
    mov %esp, (%eax)

    mov 28(%esp), %esp   // load new task's saved esp

    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    popfl             // restore EFLAGS — this is what re-enables IF on resume

    ret
