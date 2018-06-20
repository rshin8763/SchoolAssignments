TITLE MASM Fibonacci Numbers     (Program2.asm)

; Author: Ryan Shin
; Course / Project ID   271-400 / Assignment 2      Date: 1/29/17
; Description: This program will calculate and show Fibonnacci Numbers. It will show the sequence up to the 46th term
; depending on the input from the user. 

INCLUDE Irvine32.inc

; (insert constant definitions here)

.data

introMessage_1	BYTE	"Fibonacci numbers by Ryan Shin " , 0
introMessage_2	BYTE	"Please enter your name:  ", 0
introMessage_3	BYTE	"Hello, ", 0
termPrompt		BYTE	"How many Fibonacci terms do you want?  ", 0
inputVal_1		BYTE	"Outside the range. Enter a number within [1, 46] ", 0
goodbyeMessage	BYTE	"Program terminating. Goodbye, ", 0
periodChar		BYTE	".", 0
spacesString	BYTE	"      ", 0	
userName		BYTE	30 DUP(0)
repeatPrompt	BYTE	"Would you like to repeat the program?  Type '0' to quit. Type any other integer to repeat: ", 0
term			DWORD	?
num1			DWORD	1
num2			DWORD	1
column			DWORD	1 ; assign initial column index


.code
main PROC


;Introduction
	; display messages
	mov edx, OFFSET introMessage_1
	call WriteString
	call crlf
	mov edx, OFFSET introMessage_2
	call WriteString
	
;userIntroductions
	; Read user name
	mov ecx, 30
	mov edx, OFFSET userName
	call ReadString
	call crlf
	; write personal message
	mov edx, OFFSET introMessage_3
	call WriteString
	mov edx, OFFSET userName
	call WriteString
	mov edx, OFFSET periodChar
	call WriteString
	call crlf
	jmp Do ;skip input validation message

;getUserData

inputValMessage:	
	;display input validation error
	mov edx, OFFSET inputVal_1
	call WriteString
	call crlf

Do:
	; Prompt user for nth term
	mov edx, OFFSET termPrompt
	call WriteString
	call ReadInt
	mov term, eax
	cmp term, 46
	jg inputValMessage ;if out of right bound display error message and repeat validation
	cmp term, 1
	jl inputValMessage ;if out of left bound display error message and repeat validation

;displayFibs
	
	call crlf
	mov ecx, term ;set loop counter
	mov eax, num1
	call WriteDec ; Writing the first term before the loop
	mov edx, offset spacesString 
	inc column ; Cursor is now in second column so this variable is updated
	call WriteString

Calc:		
	;calculate and display next term
	mov eax, num2
	call writeDec
	add eax, num1
	
	;swap num 2 and num 1
	mov ebx, num2
	mov num1, ebx	
	
	; num 2 is set as the newest term
	mov num2, eax
	
	; insert 6 blank spaces
	call WriteString
	
	;if its the 5th column then go to the newline block
	cmp column, 5
	je newLine
	
	;else increment column
	inc column
	loop Calc
	jmp Farewell

newLine: 
	call crlf
	mov column, 1
	loop Calc

Farewell:
; Display goodbye message
	call crlf
	call crlf
	mov edx, OFFSET goodbyeMessage
	call WriteString
	mov edx, OFFSET userName
	call WriteString
	mov edx, OFFSET periodChar
	call WriteString
	call crlf
	exit 
	
main ENDP

END main
