.text
addu	$s0,$zero,$zero
addiu	$s0,$zero,3000
addiu   $s1,$zero,3105	
addu	$t4,$s0,$s1
here:	addiu	$v0,$zero,5
syscall
addu	$s3,$v0,$zero
slt	$t0,$s3,$t4
bne	$t0,$zero,here
beq	$t0,$zero,there
there:	addiu $v0,$zero,5
syscall
addu	$s0,$zero,$v0
addiu $v0,$zero,5
syscall
addu	$s1,$zero,$v0
and	$s2,$s0,$s1
addu	$a0,$s2,$zero
addiu	$v0,$zero,1
syscall
or	$s2,$s0,$s1
addu	$a0,$s2,$zero
addiu	$v0,$zero,1
syscall
subu	$s4,$s0,$s1
j	next
addiu	$t0,$zero,-30147
next:	div $s0,$s1
lw	$s5,h($gp)
sw	$s5,k($gp)
mflo	$t8
mfhi	$t9
addiu	$v0,$zero,1
addu	$a0,$zero,$t8
syscall
mult	$s0,$s1
mflo	$t8
mfhi	$t9
addiu	$v0,$zero,1
addu	$a0,$zero,$t9
syscall
addiu	$v0,$zero,10
syscall
.data
h:	.word 5
.space 3
k:	.word 11
.word -564
