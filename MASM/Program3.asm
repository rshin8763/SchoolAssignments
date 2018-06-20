TITLE MASM Integer Accumulator     (Program3.asm)

; Author: Ryan Shin
; Course / Project ID   271-400 / Assignment 3      Date: 2/12/17
; Description: This program will add up negative integers and find the rounded average.

INCLUDE Irvine32.inc

LOWER_LIMIT = -100

.data

introMessage_1	BYTE	"Integer Accumulator by Ryan Shin " , 0
introMessage_2	BYTE	"Please enter your name:  ", 0
introMessage_3	BYTE	"Hello, ", 0
Prompt			BYTE	"Please enter numbers in [-100, -1]", 0
Prompt_2		BYTE	"Enter a non-negative number when you are finished to see results.", 0
Prompt_3		BYTE	"Enter a number: ", 0
inputVal_1		BYTE	"Outside the range. Enter a number within [-100, -1] ", 0
noInputMessage	BYTE	"You entered 0 valid numbers. The sum and the average cannot be found. ", 0
numMessage_1	BYTE	"You entered " , 0
numMessage_2	BYTE	" valid numbers." , 0
sumMessage		BYTE	"The sum of your valid numbers is ", 0
avgMessage		BYTE	"The rounded average is ", 0
goodbyeMessage	BYTE	"Program terminating. Goodbye, ", 0
periodChar		BYTE	".", 0
userName		BYTE	30 DUP(0)
count			DWORD	0
sum				SDWORD	0
avg				SDWORD	?


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

;getUserData
	; Prompt user for integer
	mov edx, OFFSET Prompt
	call WriteString
	call crlf
	mov edx, OFFSET Prompt_2
	call WriteString
	call crlf
	jmp Do ;skip input validation message
	
inputValMessage:	
	;display input validation error
	mov edx, OFFSET inputVal_1
	call WriteString
	call crlf
Do:
	mov edx, OFFSET Prompt_3
	call WriteString
	call ReadInt
	cmp eax, -1
	jg Display ;if out of right bound display, exit loop
	cmp eax, LOWER_LIMIT
	jl inputValMessage; if out of left bound display error message and repeat validation
	add sum, eax
	inc count
	jmp Do; repeat Do while loop

;display results
Display:
; test to see if no valid inputs were entered. If so skip the display results block.
	cmp count, 0 
	jz NoInput

;display number of valid inputs
	call crlf
	mov edx, OFFSET numMessage_1
	call WriteString ; 
	mov eax, count
	call WriteDec 
	mov edx, OFFSET numMessage_2
	call WriteString
	call crlf

;display sum
	mov edx, OFFSET sumMessage
	call WriteString 
	mov eax, sum
	call WriteInt
	mov edx, OFFSET periodChar
	call WriteString 
	call crlf

;display average

	mov eax, sum
	mov edx, 0FFFFFFFFh ;sign extension
	idiv count
	mov avg, eax
	mov eax, edx ;
	imul eax, -2; double the remainder, and make it positive
	cmp eax, count
	jge Round ; if -2 times the remainder is greater than or equal to the count, round.
	mov edx, OFFSET avgMessage
	call WriteString 
	mov eax, avg
	call WriteInt; else display the result of integer divison
	mov edx, OFFSET periodChar
	call WriteString 
	jmp FareWell

Round:
;Round the negative average down, by decrementing it
	mov edx, OFFSET avgMessage
	call WriteString 
	dec avg
	mov eax, avg
	call WriteInt ;display rounded number
	mov edx, OFFSET periodChar
	call WriteString 
	jmp FareWell

NoInput:
; Display no input message.	
	call crlf
	mov edx, OFFSET NoInputMessage
	call WriteString

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
