TITLE MASM Composite Numbers     (Program4.asm)

; Author: Ryan Shin
; Course / Project ID   271-400 / Assignment 4      Date: 2/19/17
; Description: This program will calculate up to the 400th composite number. 

INCLUDE Irvine32.inc

UPPER_LIMIT = 400
TRUE = 1
FALSE = 0

.data

introMessage_1	BYTE	"Composite Number Calculator by Ryan Shin " , 0
introMessage_2	BYTE	"I'll accept orders for up to 400 composites. ", 0
Prompt			BYTE	"Please enter a number in [1, 400]: ", 0
inputVal_1		BYTE	"Outside the range. Enter a number within [1, 400]:  ", 0
goodbyeMessage	BYTE	"Program terminating. Goodbye. ", 0
tabSpace		BYTE	"	", 0
isCompositeFlag	BYTE	FALSE
nOfComposites	DWORD	?
testNum			DWORD	?
rowPos			DWORD	1; assign initial row position



.code

main PROC

	call introduction
	call getUserData
	call showComposites
	call farewell
	exit 
	
main ENDP

;------------------------------------------------------------------
;introduction- A procedure to print the introductory message
;receives: introMessage_1 and introMessage_2 are global variables.
;returns: Nothing
;preconditions: None
;registers changed: eax, edx
;------------------------------------------------------------------
introduction PROC; display messages
	mov edx, OFFSET introMessage_1
	call WriteString
	call crlf
	call crlf
	mov edx, OFFSET introMessage_2
	call WriteString
	call crlf
	ret
introduction ENDP
	

;------------------------------------------------------------------
;getUserData- A procedure to get the number of composite numbers within [1,400] from user
;receives: Prompt is a global string variable
;returns: Nothing
;preconditions: None
;registers changed: eax, edx
;------------------------------------------------------------------
getUserData	PROC 
	; Prompt user for integer
	mov edx, OFFSET Prompt
	call WriteString
	call inputVal
	mov nOfComposites, eax
	call crlf
	ret
getUserData	ENDP

;------------------------------------------------------------------
;inputVal- A procedure to get validate input to be within [1, 400]
;receives: inputVal_1 is a global string variable
;returns: eax = validated input
;preconditions: None
;registers changed: eax, edx
;------------------------------------------------------------------
inputVal PROC
	jmp inputValidation; skip input validation for first input

inputValMessage:	
	;display input validation error
	mov edx, OFFSET inputVal_1
	call WriteString

inputValidation:	
	call ReadInt
	cmp eax, UPPER_LIMIT
	jg inputValMessage ;if out of right bound, display error meesage and repeat validation
	cmp eax, 1
	jl inputValMessage; if out of left bound, display error message and repeat validation
	ret
inputVal ENDP
	
;------------------------------------------------------------------
;showComposites- Procedure to print the composite numbers up to the nth composite. 
;receives: nOfComposites, testNum, printFlag, rowPos are global variables
;returns: global nOfComposites = 0, testNum = nth composite
;preconditions: nOfComposites is within [1, 400]
;registers changed: eax, ecx, edx
;------------------------------------------------------------------
showComposites PROC
	mov testNum, 4 ; Set testnum as the first composite number.

L1:	; Loops for each testNumber
	mov ecx, testNum
	dec ecx; ecx is a test divisor less than the number but greater than 1.
	mov isCompositeFlag, 0 ;reset isCompositeFlag
	call isComposite
	cmp isCompositeFlag, TRUE
	jz Print ; if the current test number is composite, print
	jmp L1Test ;else test the loop condition

Print:
	dec nOfComposites
	mov eax, testNum
	call WriteDec
	mov edx, OFFSET tabSpace
	call WriteString

	cmp rowPos, 10 ; if at the final row position
	jz newLine ; Make new line
	
	;else increment rowPos and skip newLine
	inc rowPos
	jmp L1Test

newLine: 
	call crlf
	mov rowPos, 1

L1Test:
	inc testNum
	cmp nOfComposites, 0
	jz Return; If all up to the nth composite has been shown, exit procedure
	jmp L1; else repeat for the next test number.

Return:
	call crlf
	ret
showComposites ENDP


;------------------------------------------------------------------
;isComposite- Procedure to determine if the test number is composite
;receives:  testNum, isCompositeFlag are global variables
;returns: global isCompositeFlag = TRUE or FALSE
;preconditions: ecx should be a positive integer that is smaller than the test number
;registers changed: eax, ecx, edx
;-----------------------------------------------------------------

isComposite PROC
L1: ; For each test number, this loops to test all possible factors
	cmp ecx, 1 ; All numbers are divisible by 1, so we want to exit if the divisor reaches 1.
	jz Return ; This ends the loop and returns to previous procedure.
	mov edx, 0 ; zero extend  
	mov eax, testNum
	div ecx
	cmp edx, 0; check to see if any integer between [2,(testNum-1)] divides into testNum.
	jz SetFlag
	loop L1

SetFlag:
	mov isCompositeFlag, TRUE
	loop L1

Return:
	ret

isComposite ENDP

;------------------------------------------------------------------
;Farewell- Procedure to print goodbye message
;receives: goodbyeMessage is a global variable
;returns: Nothing
;preconditions: None
;registers changed: edx
;------------------------------------------------------------------
Farewell PROC
; Display goodbye message
	call crlf
	mov edx, OFFSET goodbyeMessage
	call WriteString
	call crlf
	ret
Farewell ENDP

END main