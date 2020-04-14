.equ    stack_size, 0x2000

.section  .text.startup, "ax", @progbits
.set      nomips16
.set      nomicromips
.global   _start
.ent      _start
.type     _start, @function
_start:
    la      $t0, rando_load_stage1
    la      $t1, 0x00FFFFFF
    and     $t0, $t0, $t1
    srl     $t0, $t0, 2
    la      $t1, 0x0C000000
    or      $t0, $t0, $t1
    la      $a0, load_code_stage1_hook
    sw      $t0, 0x0000($a0)
    li      $a1, 0x0004
    jal     osInvalICache
    j       0x80400000
.end _start

.global _rando_entry
.ent _rando_entry
.type _rando_entry, @function
_rando_entry:
    addiu   $sp, $sp, -0x0008
    la      $t0, _entry_data
    sw      $sp, 0x0000($t0)
    sw      $ra, 0x0004($t0)
    la      $sp, _stack + stack_size - 0x10
    jal     _main
    la      $t0, _entry_data
    lw      $sp, 0x0000($t0)
    lw      $ra, 0x0004($t0)
    addiu   $sp, $sp, 0x0008
    jr      $ra
.end _rando_entry
.size _rando_entry, . - _rando_entry

.global _rando_exit
.ent _rando_exit
.type _rando_exit, @function
_rando_exit:
    addiu   $sp, $sp, -0x0008
    sw      $ra, 0x0004($sp)
    la      $t0, _entry_data
    lw      $t1, 0x0000($t0)
    sw      $sp, 0x0000($t0)
    move    $sp, $t1
    lw      $t9, 0x0008($t0)
    jalr    $t9
    la      $t0, _entry_data
    lw      $t1, 0x0000($t0)
    sw      $sp, 0x0000($t0)
    move    $sp, $t1
    lw      $ra, 0x0004($sp)
    addiu   $sp, $sp, 0x0008
    jr      $ra
.end _rando_exit
.size _rando_exit, . - _rando_exit

.section .stack, "aw", @nobits
.align 3
.type _stack, @object
.local _stack
_stack:
    .space stack_size
.size _stack, . - _stack

.section .data, "aw", @progbits
.align 2
.type _entry_data, @object
.global _entry_data
_entry_data:
    .space 0xC
.size _entry_data, . - _entry_data
