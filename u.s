!------------- u.s file --------------------
.globl _getcs, _getds,_setds
.globl _main0,_prints,_syscall,_exit
.globl _int80, _mgets
				
export auto_start
auto_start:	

start:
        call _main0
	
! exit(0) for normal termination
        push  #0
        call  _exit

dead:   jmp   dead

_mgets:

_getcs:
        mov   ax, cs
        ret
_getds: mov   ax, ds
        ret
_setds:
	push  bp
	mov   bp,sp
	mov   ax,4[bp]
	mov   ds,ax
	mov   sp,bp
	pop   bp
	ret
	
	
_syscall:
	
_int80:
	push   bp
	mov    bp,sp
	int    80
	mov    sp,bp
	pop    bp
	ret

