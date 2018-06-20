TITLE MASM Arithmatic     (Program1.asm)

; Author: Ryan Shin
; Course / Project ID   271-400 / Assignment 1      Date: 1/22/17
; Description: This program will show the sum, difference,
; product, quotient, and remainder of two numbers entered by the user.

INCLUDE Irvine32.inc

; (insert constant definitions here)

.data

eCMessage_1		BYTE	"**EC: Program Repeats until the user decides to quit.", 0
eCMessage_2		BYTE	"**EC: Program verifies second number is less than first.", 0
introMessage_1	BYTE	"Elementary Arithmatic by Ryan Shin " , 0
introMessage_2	BYTE	"Enter 2 natural numbers, and I'll calculate the sum, difference,", 0
introMessage_3	BYTE	"product, quotient, and remainder", 0
goodbyeMessage	BYTE	"Program terminating. Goodbye.", 0
inputMessage_1	BYTE	"First Number: ", 0
inputMessage_2	BYTE	"Second Number (must be less than the first and not zero): ", 0
results1		BYTE	" + " , 0
results2		BYTE	" - " , 0
results3		BYTE	" x " , 0
results4		BYTE	" € " , 0
results5		BYTE	" remainder ", 0
results6		BYTE	" = ", 0
repeatPrompt	BYTE	"Would you like to repeat the program?  Type '0' to quit. Type any other integer to repeat: ", 0
num1			DWORD	?
num2			DWORD	?
sum				DWORD	?
difference		DWORD	?
product			DWORD	?
quotient		DWORD	?
remainder		DWORD	?

.code
main PROC


; Print Intro message and EC messages
	mov edx, OFFSET introMessage_1
	call WriteString
	call crlf
	mov edx, OFFSET eCMessage_1
	call WriteString
	call crlf
	mov edx, OFFSET eCMessage_2
	call WriteString
	call crlf
	call crlf
	mov edx, OFFSET introMessage_2
	call WriteString
	call crlf
	mov edx, OFFSET introMessage_3
	call WriteString
	call crlf

RepeatLabel:
; Prompt user for inputs
	call crlf
	mov edx, OFFSET inputMessage_1
	call WriteString
	call ReadInt
	mov num1, eax

InputVal:
; Will repeat input promps if num 2 >= num 1, or if num 2 == 0. 
	mov edx, OFFSET inputMessage_2
	call WriteString
	call ReadInt
	cmp num1, eax
	mov num2, eax
	jle InputVal
	cmp eax, 0
	je InputVal
	call crlf


; Calculate the sum
	mov eax, num1
	add eax, num2
	mov sum, eax

; Calculate the difference
	mov eax, num1
	sub eax, num2
	mov difference, eax

; Calculate the the product
	mov eax, num1
	mul num2
	mov product, eax

; Calculate the quotient and remainder
	mov eax, num1
	mov edx, 0
	div num2
	mov quotient, eax
	mov remainder, edx

; Print the sum
	mov eax, num1
	call WriteDec
	
	mov edx, OFFSET results1
	call WriteString

	mov eax, num2
	call WriteDec

	mov edx, OFFSET results6
	call WriteString
	
	mov eax, sum
	call WriteDec
	call crlf

; Print the difference
	mov eax, num1
	call WriteDec
	
	mov edx, OFFSET results2
	call WriteString

	mov eax, num2
	call WriteDec

	mov edx, OFFSET results6
	call WriteString
	
	mov eax, difference
	call WriteDec
	call crlf

; Print the product
	mov eax, num1
	call WriteDec
	
	mov edx, OFFSET results3
	call WriteString

	mov eax, num2
	call WriteDec

	mov edx, OFFSET results6
	call WriteString

	mov eax, product
	call WriteDec
	call crlf

; Print the quotient
	mov eax, num1
	call WriteDec
	
	mov edx, OFFSET results4
	call WriteString

	mov eax, num2
	call WriteDec

	mov edx, OFFSET results6
	call WriteString
	
	mov eax, quotient
	call WriteDec

	mov edx, OFFSET results5
	call WriteString
	
	mov eax, remainder
	call WriteDec
	call crlf

; Display repeat program prompt
	call crlf
	mov edx, OFFSET repeatPrompt
	call WriteString

; Check response to repeat prompt. If 0, quit program.
	call ReadInt
	cmp eax, 0
	jz Goodbye
	jmp RepeatLabel


Goodbye:
; Display goodbye message
	mov edx, OFFSET goodbyeMessage
	call WriteString
	call crlf

	exit	; exit to operating system
main ENDP

; (insert additional procedures here)

END main
