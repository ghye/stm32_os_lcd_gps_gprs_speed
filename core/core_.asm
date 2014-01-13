;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;�������������������
;;;;����:2011-09-16
;;;;��д:mxd
;;;;�汾:1.0
;;;;ʵ��˼��:���ucos�����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

NVIC_INT_CTRL   	EQU     0xE000ED04  ; �жϿ��ƼĴ���
NVIC_SYSPRI2    		EQU     0xE000ED20  ; ϵͳ���ȼ��Ĵ���2
NVIC_PENDSV_PRI 	EQU     0xFFFF0000   ; ����жϺ�ϵͳ�����ж�
NVIC_PENDSVSET  	EQU     0x10000000  ; ��������жϵ�ֵ

	IMPORT  OS_CUR_SP     ;��ǰ�����ջָ��
	IMPORT  OS_NEXT_SP    ;�¸���Ҫ���е������ջָ��

	EXPORT  OS_CONTEX_SWITCH   ;�������л�
        	EXPORT  OS_START                  ;��ʼ���������

	EXPORT  PendSV_Handler	;��Ҫ��������������ϵͳĬ�Ͻӿ�
	PRESERVE8
	AREA    |.text|, CODE, READONLY
	THUMB

OS_CONTEX_SWITCH
	PUSH     {R4, R5}
	LDR       R4, =NVIC_INT_CTRL	;�˹�����PendSV�쳣���ο�ucos!!!
	LDR       R5, =NVIC_PENDSVSET
	STR       R5, [R4]
	POP       {R4, R5}
	BX         LR

OS_START
	LDR     R4, =NVIC_SYSPRI2      ; set the PendSV exception priority
	LDR     R5, =NVIC_PENDSV_PRI
	STR     R5, [R4]

	MOV     R4, #0                 ; set the PSP to 0 for initial context switch call
	MSR     PSP, R4

	LDR     R4, =NVIC_INT_CTRL     ;rigger the PendSV exception (causes context switch)
	LDR     R5, =NVIC_PENDSVSET
	STR     R5, [R4]

	CPSIE   I        ;enable interrupts at processor level

	B    .            ;should never get here

PendSV_Handler
	CPSID   I                                                   ; Prevent interruption during context switch
	MRS     R0, PSP                                             ; PSP is process stack pointer 
	CBZ     R0, Skip_Save		                    ; Skip register save the first time

	SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack
	STM     R0, {R4-R11}

	LDR     R1, =OS_CUR_SP                                       ; OSTCBCur->OSTCBStkPtr = SP
	LDR     R1, [R1]
	STR     R0, [R1]                                            ; R0 is SP of process being switched out

Skip_Save
	LDR     R0, =OS_CUR_SP                                       ; OSTCBCur  = OSTCBHighRdy;
	LDR     R1, =OS_NEXT_SP
	LDR     R2, [R1]
	STR     R2, [R0]

	LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
	LDM     R0, {R4-R11}                                        ; Restore r4-11 from new process stack
	ADDS   R0, R0, #0x20
	MSR     PSP, R0                                             ; Load PSP with new process SP
	ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack
	CPSIE   I
	BX      LR                                                  ; Exception return will restore remaining context

	END
	
