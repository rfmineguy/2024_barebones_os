.global irq_asm_default
.global irq_asm_keyboard
.global irq_asm_timer

.extern irq_default
.extern irq_keyboard
.extern irq_timer

irq_asm_default:
    call irq_default
    iret

irq_asm_keyboard:
    call irq_keyboard
    iret

irq_asm_timer:
    call irq_timer
    iret
