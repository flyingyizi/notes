
/*
 * MOV (Move) writes a value to the destination register. The value can be
 * either an immediate value or a value from a register, and can be shifted
 * before the write.
 *
 * MOV can optionally update the condition code flags, based on the result.
 *
 * Syntax
 *
 *   MOV {<cond>} {S} <Rd>, <shifter_operand>
 *
 * Usage
 *
 * - Move a value from one register to another.
 * - Put a constant value into a register.
 * - Perform a shift without any other arithmetic or logical operation. Use 
 *   a left shift by n to multiply by 2^n .
 * - When the PC is the destination of the instruction, a branch occurs. The
 *   instruction:
 *           MOV PC, LR
 *   can therefore be used to return from a subroutine (see instructions B, 
 *   BL on page A4-10). In T variants of architecture 4 and in architecture 
 *   5 and above, the instruction BX LR must be used in place of MOV PC, LR, 
 *   as the BX instruction automatically switches back to Thumb state if 
 *   appropriate (but see also The T and J bits on page A2-15 for operation
 *   on non-T variants of ARM architecture version 5).
 * - When the PC is the destination of the instruction and the S bit is set,
 *   a branch occurs and the SPSR of the current mode is copied to the CPSR.
 *   This means that you can use a MOVS PC, LR instruction to return from 
 *   some types of exception (see Exceptions on page A2-16).
 */
// https://github.com/BiscuitOS/HardStack/blob/master/Language/Assembly/ARM-GNU-Assembly/Instruction/mov/asm.c

static int debug_mov(void)
{
	unsigned long reg;
	unsigned long val = 0x3;

	/* NOP */
	__asm__ volatile ("mov r0, r0");

	/* Put a constant value into a register */
	// __asm__ volatile ("mov r0, %0" :: "r" (val));
    __asm__ volatile ("mov r0, %[value]" :: [value] "r" (val));

	/* Move a value from one register to another */
	// __asm__ volatile ("mov %0, r0": "=r" (reg));
	__asm__ volatile ("mov %[result], r0": [result]"=r" (reg));
	printf("Register R0: %#lx\n", reg);

	/* Perform a shift without any other arithmetic or logical operation.
	 * Use a left shift by n to multiply by 2^n.
	 * R0 = R1 * 8 */
	__asm__ volatile ("mov r1, #2"); /* r1=2 */
	__asm__ volatile ("mov r0, r1, lsl #3"); /* r1=2, r0=16 */
	__asm__ volatile ("mov %0, r0" : "=r" (reg)); /* r0=16 */
	printf("Register R0 << 3: %#lx\n", reg);

	return 0;
}

/*
 * MCR (Move to Coprocessor from ARM Register) passes the value of 
 * register <Rd> to the coprocessor whose number is cp_num. If no 
 * coprocessors indicate that they can execute the instruction, an 
 * Undefined Instruction exception is generated.
 *
 * Syntax
 *   MCR{<cond>} <coproc>, <opcode_1>, <Rd>, <CRn>, <CRm>{, <opcode_2>}
 *   MCR2        <coproc>, <opcode_1>, <Rd>, <CRn>, <CRm>{, <opcode_2>}
 * 
 * SHOULD BE IN secure svc mode TO EXECUTE IT
 */

static int debug_mcr(void)
{
	unsigned long CSS;

	/* Read CSSELR into Rt */
	// __asm__ volatile ("mrc p15,2,%0,c0,c0,0" : "=r" (CSS));
	__asm__ volatile ("mrc p15,2,%[result],c0,c0,0" : [result]"=r" (CSS));
	printf("CSSELR: %#lx\n", CSS);

	/* Write Rt into CSSELR */
	// __asm__ volatile ("mcr p15,2,%0,c0,c0,0" :: "r" (CSS));
	__asm__ volatile ("mcr p15,2,%[value],c0,c0,0" :: [value]"r" (CSS));

	return 0;
}

/*
 * MLA (Multiply Accumulate) multiplies two signed or unsigned 32-bit 
 * values, and adds a third 32-bit value. The least significant 32 bits
 * of the result are written to the destination register. MLA can 
 * optionally update the condition code flags, based on the result.
 *
 * Syntax
 *   MLA{<cond>}{S} <Rd>, <Rm>, <Rs>, <Rn>
 */

static int debug_mla(void)
{
	unsigned long R1 = 0x2;
	unsigned long R2 = 0x3;
	unsigned long R3 = 0x4;
	unsigned long total;

	/* MLA R1, R2, R3, R4 ---> R1 = (R2 * R3) + R4 */
	__asm__ volatile ("mla %0, %1, %2, %3"
			: "=r" (total) : "r" (R1), "r" (R2), "r" (R3));

	printf("(%#lx * %#lx) + %#lx = %#lx\n", R1, R2, R3, total);

	return 0;
}

/*
 * MRS (Move PSR to general-purpose register) moves the value of the CPSR 
 * or the SPSR of the current mode into a general-purpose register. In the
 * general-purpose register, the value can be examined or manipulated with
 * normal data-processing instructions.
 *
 * Syntax
 *   MRS{<cond>} <Rd>, CPSR
 *   MRS{<cond>} <Rd>, SPSR
 *
 * 32        27         25     20        16        10        5        0
 * +-+-+-+-+-+----------+-+----+---------+---------+-+-+-+-+-+--------+
 * | | | | | |          | |    |         |         | | | | | |        |
 * |N|Z|C|V|Q| IT[1:0]  |J|    | GE[3:0] | IT[7:2] |E|A|I|F|T| M[4:0] |
 * | | | | | |          | |    |         |         | | | | | |        |
 * +-+-+-+-+-+----------+-+----+---------+---------+-+-+-+-+-+--------+
 * | <-----> |                                       | <-> |
 *  Condition                                       Mask bits
 *    flags
 *
 * Condition flags, bits[31:28]
 *
 *   N,bit[31]       Negative condition flag
 *   Z,bit[30]       Zero condition flag
 *   C,bit[29]       Carry condition flag
 *   V,bit[28]       Overflow condition flag
 *
 * Q,bit[27]
 *   
 *   Cumulative saturation bit.
 *
 * IT[7:0],bits[15:10,26:25]
 *
 *   If-Then execution state bits for the Thumb IT(if-then) instruction.
 *
 * GE[3:0],bits[19:16]
 *
 *   Greater then or Equal flags.
 *
 * E,bit[9]
 *
 *   Endlianness execution state bit.
 *   0     Little-endian operation
 *   1     Big-endian operation
 *
 * Mask bits,bits[8:6]
 *
 *   A,bit[8]        Asynchronous abort mask bit.
 *   I,bit[7]        IRQ mask bit.
 *   F,bit[6]        FIQ mask bit.
 *
 *   The possible value of each bit are:
 *
 *   0               Exception not masked
 *   1               Exception masked
 *
 * T,bit[5] 
 *
 *   Thumb execution state bit.
 *
 * M[4:0],bits[4:0]
 *
 *   Mode field. This field determines the current mode of the processor.
 */

static int debug_mrs(void)
{
	unsigned long CPSR;

	/* Read CPSR register */
	__asm__ volatile ("mrs %0, cpsr" : "=r" (CPSR));
	printf("CPSR: %#lx\n", CPSR);
	

	return 0;
}

/*
 * MSR (Move to Status Register from ARM Register) transfers the value of 
 * a general-purpose register or an immediate constant to the CPSR or the
 * SPSR of the current mode.
 *
 * Syntax
 *   MSR{<cond>} CPSR_<fields>, #<immediate>
 *   MSR{<cond>} CPSR_<fields>, <Rm>
 *   MSR{<cond>} SPSR_<fields>, #<immediate>
 *   MSR{<cond>} SPSR_<fields>, <Rm>
 *
 * 32        27         25     20        16        10        5        0
 * +-+-+-+-+-+----------+-+----+---------+---------+-+-+-+-+-+--------+
 * | | | | | |          | |    |         |         | | | | | |        |
 * |N|Z|C|V|Q| IT[1:0]  |J|    | GE[3:0] | IT[7:2] |E|A|I|F|T| M[4:0] |
 * | | | | | |          | |    |         |         | | | | | |        |
 * +-+-+-+-+-+----------+-+----+---------+---------+-+-+-+-+-+--------+
 * | <-----> |                                       | <-> |
 *  Condition                                       Mask bits
 *    flags
 *
 * Condition flags, bits[31:28]
 *
 *   N,bit[31]       Negative condition flag
 *   Z,bit[30]       Zero condition flag
 *   C,bit[29]       Carry condition flag
 *   V,bit[28]       Overflow condition flag
 *
 * Q,bit[27]
 *   
 *   Cumulative saturation bit.
 *
 * IT[7:0],bits[15:10,26:25]
 *
 *   If-Then execution state bits for the Thumb IT(if-then) instruction.
 *
 * GE[3:0],bits[19:16]
 *
 *   Greater then or Equal flags.
 *
 * E,bit[9]
 *
 *   Endlianness execution state bit.
 *   0     Little-endian operation
 *   1     Big-endian operation
 *
 * Mask bits,bits[8:6]
 *
 *   A,bit[8]        Asynchronous abort mask bit.
 *   I,bit[7]        IRQ mask bit.
 *   F,bit[6]        FIQ mask bit.
 *
 *   The possible value of each bit are:
 *
 *   0               Exception not masked
 *   1               Exception masked
 *
 * T,bit[5] 
 *
 *   Thumb execution state bit.
 *
 * M[4:0],bits[4:0]
 *
 *   Mode field. This field determines the current mode of the processor.
 */

static int debug_msr(void)
{
	unsigned long CPSR;

	/* Read CPSR register */
	__asm__ volatile ("mrs %0, cpsr" : "=r" (CPSR));
	printf("CPSR: %#lx\n", CPSR);
	/* Write data into CPSR register */
	__asm__ volatile ("msr cpsr, %0" :: "r" (CPSR));

	return 0;
}

/*
 * MUL (Multiply) multiplies two signed or unsigned 32-bit values. The
 * least significant 32 bits of the result are written to the destination
 * register. MUL can optionally update the condition code flags, based on
 * the result.
 *
 * Syntax
 *   MUL{<cond>}{S} <Rd>, <Rm>, <Rs>
 */

static int debug_mul(void)
{
	unsigned long R1 = 0x2;
	unsigned long R2 = 0x3;
	unsigned long total;

	/* MUL R1, R2, R3 ---> R1 = R2 * R3 */
	__asm__ volatile ("mul %0, %1, %2"
			: "=r" (total) : "r" (R1), "r" (R2));

	printf("%#lx * %#lx = %#lx\n", R1, R2, total);

	return 0;
}

/*
 * MVN (Move Not) generates the logical ones complement of a value. The
 * value can be either an immediate value or a value from a register, 
 * and can be shifted before the MVN operation. MVN can optionally update
 * the condition code flags, based on the result.
 *
 * Syntax
 *   MVN{<cond>} {S} <Rd>, <shifter_operand>
 */

static int debug_mvn(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 0xb;

	/* MVN R0, R1---> R0 = !R1 */
	__asm__ volatile ("mvn %0, %1" :: 
		"r" (a0), "r" (a1));

	printf("!%#lx = %#lx\n", a1, a0);

	return 0;
}

/*
 * ADC (Add with Carry) adds two values and the Carry flag. The first value 
 * comes from a register. The second value can be either an immediate value 
 * or a value from a register, and can be shifted before the addition. ADC 
 * can optionally update the condition code flags, based on the result.
 *
 * Syntax
 * ADC {<cond>} {S} <Rd>, <Rn>, <shifter_operand>
 */

static int debug_adc(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 2;
	unsigned long a2 = 1;

	/* ADC R0, R1, R2 ---> R0 = R1 + R2 + 1 */
	__asm__ volatile ("adc %0, %1, %2" :: 
		"r" (a0), "r" (a1), "r" (a2));

	printf("%#lx + %#lx = %#lx\n", a1, a2, a0);

	return 0;
}

/*
 * ADD adds two values. The first value comes from a register. The second 
 * value can be either an immediate value or a value from a register, and
 * can be shifted before the addition. ADD can optionally update the 
 * condition code flags, based on the result.
 *
 * Syntax
 *  ADD {<cond>} {S} <Rd>, <Rn>, <shifter_operand>
 *
 * Usage
 *
 * - Use ADD to add two values together.
 * - To increment a register value in Rx use:
 *        ADD Rx, Rx, #1
 * - You can perform constant multiplication of Rx by 2 n +1 into Rd with:
 *        ADD Rd, Rx, Rx, LSL #n
 * - To form a PC-relative address use:
 *        ADD Rd, PC, #offset
 *   where the offset must be the difference between the required address 
 *   and the address held in the PC, where the PC is the address of the 
 *   ADD instruction itself plus 8 bytes.
 */

static int debug_add(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 2;
	unsigned long a2 = 1;

	/* ADD R0, R1, R2 ---> R0 = R1 + R2 */
	__asm__ volatile ("add %0, %1, %2" :: 
		"r" (a0), "r" (a1), "r" (a2));

	printf("%#lx + %#lx = %#lx\n", a1, a2, a0);

	a0 = 0;
	/* ADD R0, R1, R2, LSL #n --> R0 = R1 + (R2 << n) */
	__asm__ volatile ("add %0, %1, %2, lsl #3" ::
		"r" (a0), "r" (a1), "r" (a2));

	printf("%#lx + %#lx << 3 = %#lx\n", a1, a2, a0);

	return 0;
}

/*
 * AND performs a bitwise AND of two values. The first value comes from a 
 * register. The second value can be either an immediate value or a value 
 * from a register, and can be shifted before the AND operation. AND can 
 * optionally update the condition code flags, based on the result.
 *
 * Syntax
 *  AND{<cond>} {S} <Rd>, <Rn>, <shifter_operand>
 */

static int debug_and(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 0x23;
	unsigned long a2 = 0x3;

	/* AND R0, R1, R2 ---> R0 = R1 & R2  */
	__asm__ volatile ("and %0, %1, %2" :: 
		"r" (a0), "r" (a1), "r" (a2));

	printf("%#lx && %#lx = %#lx\n", a1, a2, a0);

	return 0;
}

/*
 * Arithmetic Shift Left
 *
 * Syntax
 * 
 * RX, ASL #n 
 * or
 * RX, ASL Rn
 */

static int debug_asl(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 0x1;

	/* MOV R0, R1, ASL #3 --> R0 = R1 << 3 */
	__asm__ volatile ("mov %0, %1, asl #3"
		: "=r" (a0) : "r" (a1));

	printf("%#lx << 3 = %#lx\n", a1, a0);

	return 0;
}


/*
 * ASR Arithmetic Shift right
 *
 * Syntax
 * 
 * RX, ASR #n 
 * or
 * RX, ASR Rn
 */

static int debug_asr(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 0x8;

	/* MOV R0, R1, ASR #3 --> R0 = R1 >> 3 */
	__asm__ volatile ("mov %0, %1, asr #3"
		: "=r" (a0) : "r" (a1));

	printf("%#lx >> 3 = %#lx\n", a1, a0);

	return 0;
}

/*
 * B (Branch) cause a branch to a target address, and provide both 
 * conditional and unconditional changes to program flow.
 *
 * Syntax
 *   B{L}{<cond>} <target_address>
 *
 * BEQ ---> ==  ---> equal
 * BNE ---> !=  ---> Unequal
 * BPL ---> > 0 ---> N0nNegative number
 * BMI ---> < 0 ---> Negative number
 * BCC ---> !C  ---> Non-Carry
 * BCS ---> C   ---> Carry
 * BLO ---> < 0 ---> Less then with unsigned-
 * BHS ---> >=0 ---> Big then and equal with unsigned-
 * BHI ---> >   ---> Big then with unsigned-
 * BLS ---> <=  ---> Less than and equal with unsigned-
 * BVC ---> !O  ---> No overflow with signed-
 * BVS ---> O   ---> Overflow with signed-
 * BGT ---> >   ---> Big then with signed-
 * BGE ---> >=  ---> Big and equal with signed-
 * BLT ---> <   ---> Less then with signed-
 * BLE ---> <=  ---> Less and equal with signed-
 */

static int debug_b(void)
{
	unsigned long RET;

	__asm__ volatile ("b _br\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_br:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET));

	printf("BR: %#lx\n", RET);

	return 0;
}

/*
 * BCC ---> !C  ---> Non-Carry
 *
 * Syntax
 *   BCC <Branch>
 */

static int debug_bcc(void)
{
	unsigned long R1 = 0x2;
	unsigned long R2 = 0x5;
	unsigned long RET;

	/* BCC <branch> --> No Carry and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bcc _nc\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_nc:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx - %#lx no Carry!", R1, R2);
	else
		printf("%#lx - %#lx Carry!", R1, R2);

	return 0;
}

/*
 * BCS ---> C   ---> Carry
 *
 * Syntax
 *   BCS <Branch>
 */

static int debug_bcs(void)
{
	unsigned long R1 = 0xffff;
	unsigned long R2 = 0x5;
	unsigned long RET;

	/* BCS <branch> --> Carry and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bcs _c\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_c:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx - %#lx Carry!", R1, R2);
	else
		printf("%#lx - %#lx no Carry!", R1, R2);

	return 0;
}

/*
 * BEQ ---> ==  ---> equal
 *
 * Syntax
 *   BEQ <Branch>
 */

static int debug_beq(void)
{
	unsigned long R1 = 0x8;
	unsigned long R2 = 0x8;
	unsigned long RET;

	/* BEQ <branch> --> equal and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "beq _equ\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_equ:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx == %#lx", R1, R2);
	else
		printf("%#lx != %#lx", R1, R2);

	return 0;
}


/*
 * BGT ---> >   ---> Big then with signed-
 *
 * Syntax
 *   BGT <Branch>
 */

static int debug_bgt(void)
{
	long R1 = -32;
	long R2 = -45;
	unsigned long RET;

	/* BGT <branch> --> Big then and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bgt _gt\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_gt:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx > %#lx", R1, R2);
	else
		printf("%#lx < %#lx", R1, R2);

	return 0;
}

/*
 * BVS ---> O   ---> Overflow with signed-
 *
 * Syntax
 *   BVS <Branch>
 */

static int debug_bvs(void)
{
	long R1 = -324234;
	long R2 = 45;
	unsigned long RET;

	/* BVS <branch> --> Overflow and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bvs _vs\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_vs:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx - %#lx Overflow", R1, R2);
	else
		printf("%#lx - %#lx not Overflow", R1, R2);

	return 0;
}

/*
 * BVC ---> !O  ---> No overflow with signed-
 *
 * Syntax
 *   BVC <Branch>
 */

static int debug_bvc(void)
{
	long R1 = -324234;
	long R2 = 45;
	unsigned long RET;

	/* BVC <branch> --> No overflow and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bvc _vc\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_vc:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx - %#lx not Overflow", R1, R2);
	else
		printf("%#lx - %#lx Overflow", R1, R2);

	return 0;
}

/*
 * BPL ---> > 0 ---> N0nNegative number
 *
 * Syntax
 *   BPL <Branch>
 */

static int debug_bpl(void)
{
	unsigned long R1 = 0x8;
	unsigned long R2 = 0x2;
	unsigned long RET;

	/* BPL <branch> --> NoNegative number and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bpl _nng\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_nng:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx - %#lx >= 0", R1, R2);
	else
		printf("%#lx - %#lx < 0", R1, R2);

	return 0;
}

/*
 * BNE ---> !=  ---> Unequal
 *
 * Syntax
 *   BNE <Branch>
 */

static int debug_bne(void)
{
	unsigned long R1 = 0x8;
	unsigned long R2 = 0x5;
	unsigned long RET;

	/* BNE <branch> --> Un-equal and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bne _ne\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_ne:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx != %#lx", R1, R2);
	else
		printf("%#lx == %#lx", R1, R2);

	return 0;
}

/*
 * BMI ---> < 0 ---> Negative number
 *
 * Syntax
 *   BMI <Branch>
 */

static int debug_bmi(void)
{
	unsigned long R1 = 0x3;
	unsigned long R2 = 0x5;
	unsigned long RET;

	/* BMI <branch> --> Negative number and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bmi _ng\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_ng:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx - %#lx < 0", R1, R2);
	else
		printf("%#lx - %#lx >= 0", R1, R2);

	return 0;
}


/*
 * BLT ---> <   ---> Less then with signed-
 *
 * Syntax
 *   BLT <Branch>
 */

static int debug_blt(void)
{
	long R1 = -50;
	long R2 = -45;
	unsigned long RET;

	/* BLT <branch> --> Less than and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "blt _lt\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_lt:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%ld < %ld", R1, R2);
	else
		printf("%ld > %ld", R1, R2);

	return 0;
}


/*
 * BLO ---> < 0 ---> Less then with unsigned-
 *
 * Syntax
 *   BLO <Branch>
 */

static int debug_blo(void)
{
	unsigned long R1 = 0x2;
	unsigned long R2 = 0x5;
	unsigned long RET;

	/* BLO <branch> --> Less and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "blo _le\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_le:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx < %#lx", R1, R2);
	else
		printf("%#lx >= %#lx", R1, R2);

	return 0;
}


/*
 * BLE ---> <=  ---> Less and equal with signed-
 *
 * Syntax
 *   BLE <Branch>
 */

static int debug_ble(void)
{
	long R1 = -50;
	long R2 = -45;
	unsigned long RET;

	/* BLE <branch> --> Less then and equal, then jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "ble _le\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_le:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%ld <= %ld", R1, R2);
	else
		printf("%ld > %ld", R1, R2);

	return 0;
}


/*
 * BL (Branch and Link) cause a branch to a target address, and provide
 * both conditional and unconditional changes to program flow. BL also 
 * stores a return address in the link register, R14 (also known as LR).
 *
 * Syntax
 *   B{L}{<cond>} <target_address>
 */

int debug_show(void)
{
	printf("Hello World\n");
	return 0;
}

static int debug_bl(void)
{
	__asm__ volatile ("bl debug_show");

	return 0;
}


/*
 * BIC (Bit Clear) performs a bitwise AND of one value with the complement 
 * of a second value. The first value comes from a register. The second 
 * value can be either an immediate value or a value from a register, and 
 * can be shifted before the BIC operation. BIC can optionally update the 
 *condition code flags, based on the result.
 *
 * Syntax
 *   BIC{<cond>} {S} <Rd>, <Rn>, <shifter_operand>
 */

/* Module initialize entry */
static int __init bic_init(void)
{

	unsigned long a0 = 0;
	unsigned long a1 = 0xFFFF;
	unsigned long a2 = 0xb;

	/* BIC R0, R1, R2 ---> R0 = R1 Clear bit on R2  */
	__asm__ volatile ("bic r0, %1, %2\n\r" 
			  "mov %0, r0"
			: "=r" (a0)
        		: "r" (a1), "r" (a2));

	printf("%#lx Clear %#lx = %#lx\n", a1, a2, a0);

	return 0;
}

/*
 * BHS ---> >=0 ---> Big then and equal with unsigned-
 *
 * Syntax
 *   BHS <Branch>
 */

static int debug_bhs(void)
{
	unsigned long R1 = 0x8;
	unsigned long R2 = 0x5;
	unsigned long RET;

	/* BHS <branch> --> big then and equal, then jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bhs _be\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_be:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx >= %#lx", R1, R2);
	else
		printf("%#lx < %#lx", R1, R2);

	return 0;
}


/*
 * BHI ---> >   ---> Big then with unsigned-
 *
 * Syntax
 *   BHI <branch>
 */

static int debug_bhi(void)
{
	unsigned long R1 = 0x2;
	unsigned long R2 = 0x1;
	unsigned long RET;

	/* BHI <branch> --> Big and jump to branck */
	__asm__ volatile ("cmp %1, %2\n\r"
			  "bhi _big\n\r"
			  "mov %0, #0\n\r"
			  "b out\n\r"
		"_big:\n\r"
			  "mov %0, #1\n\r"
		"out:\n\r"
			  "nop"
			: "=r" (RET)
			: "r" (R1), "r" (R2));

	if (RET)
		printf("%#lx > %#lx", R1, R2);
	else
		printf("%#lx < %#lx", R1, R2);

	return 0;
}


/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmda(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMIA: Load memory into Register, and descrement After
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |            -|------------> R3
	 *    R0[5]--> +-------------+
	 *             |            -|------------> R2
	 *             +-------------+
	 *             |            -|------------> R1
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop stack into registe.
	 */
	__asm__ volatile ("ldmda %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}

/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmdb(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMIA: Load memory into Register, and descrement Before
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |            -|------------> R3
	 *             +-------------+
	 *             |            -|------------> R2
	 *             +-------------+
	 *             |            -|------------> R1
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop stack into registe.
	 */
	__asm__ volatile ("ldmdb %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}


/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmea(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMEA: Load memory into Register, and Empty Ascending
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |            -|------------> R3
	 *             +-------------+
	 *             |            -|------------> R2
	 *             +-------------+
	 *             |            -|------------> R1
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop empty ascending stack into registe.
	 */
	__asm__ volatile ("ldmea %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}


/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmed(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMED: Load memory into Register, and Empty Descending
	 *
	 *
	 *             +-------------+
	 *             |            -|-----------> R3
	 *             +-------------+
	 *             |            -|-----------> R2
	 *             +-------------+
	 *             |            -|-----------> R1
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop empty descending stack into registe.
	 */
	__asm__ volatile ("ldmed %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}


/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmfa(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMFA: Load memory into Register, and Full Ascending stack
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |            -|------------> R3
	 *    R0[5]--> +-------------+
	 *             |            -|------------> R2
	 *             +-------------+
	 *             |            -|------------> R1
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop full ascending stack into registe.
	 */
	__asm__ volatile ("ldmfa %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}


/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmfd(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMFD: Load memory into Register, and Full Descending
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |            -|------------> R3
	 *             +-------------+
	 *             |            -|------------> R2
	 *             +-------------+
	 *             |            -|------------> R1
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop full descending stack into registe.
	 */
	__asm__ volatile ("ldmfd %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}


/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmia(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMIA: Load memory into Register, and increment After
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |            -|---------> R3
	 *             +-------------+
	 *             |            -|---------> R2
	 *             +-------------+
	 *             |            -|---------> R1
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop stack into registe.
	 */
	__asm__ volatile ("ldmia %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}


/*
 * LDM (Load Multiple) loads a non-empty subset, or possibly all, of the
 * general-purpose registers from sequential memory locations. It is 
 * useful for block loads, stack operations and procedure exit sequences.
 * The general-purpose registers loaded can include the PC. If they do, 
 * the word loaded for the PC is treated as an address and a branch occurs
 * to that address. In ARMv5 and above, bit[0] of the loaded value
 * determines whether execution continues after this branch in ARM state
 * or in Thumb state, as though a BX (loaded_value) instruction had been
 * executed (but see also The T and J bits on page A2-15 for operation on
 * non-T variants of ARMv5). In earlier versions of the architecture, 
 * bits[1:0] of the loaded value are ignored and execution continues in 
 * ARM state, as though the instruction MOV PC,(loaded_value) had been
 * executed.
 *
 * Syntax
 *   LDM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_ldmib(void)
{
static unsigned long R0[10] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
				0x77, 0x88, 0x99, 0xaa};
	unsigned long R1 = 0x0;
	unsigned long R2 = 0x0;
	unsigned long R3 = 0x0;
	int i;
	
	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);
	/*
	 * LDMIB: Load memory into Register, and increment Before
	 *
	 *
	 *             +-------------+
	 *             |            -|---------> R3
	 *             +-------------+
	 *             |            -|---------> R2
	 *             +-------------+
	 *             |            -|---------> R1
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Pop stack into registe.
	 */
	__asm__ volatile ("ldmib %3!, {%0, %1, %2}"
			: "=r" (R1), "=r" (R2), "=r" (R3)
			: "r" (&R0[5]));

	printf("R1: %#lx R2: %#lx R3: %#lx\n", R1, R2, R3);

	return 0;
}

/*
 * LDR (Load Register) loads a word from a memory address. If the PC is 
 * specified as register <Rd> , the instruction loads a data word which 
 * it treats as an address, then branches to that address. In ARMv5T and
 * above, bit[0] of the loaded value determines whether execution continues
 * after this branch in ARM state or in Thumb state, as though a BX (
 * loaded_value) instruction had been executed. In earlier versions of
 * the architecture, bits[1:0] of the loaded value are ignored and 
 * execution continues in ARM state, as though a MOV PC,(loaded_value)
 * instruction had been executed.
 *
 * Syntax
 *   LDR{<cond>} <Rd>, <addressing_mode>
 *
 * Usage
 *
 * - Using the PC as the base register allows PC-relative addressing, which
 *   facilitates position-independent code. Combined with a suitable 
 *   addressing mode, LDR allows 32-bit memory data to be loaded into a
 *   general-purpose register where its value can be manipulated. If the
 *   destination register is the PC, this instruction loads a 32-bit address
 *   from memory and branches to that address.
 * - To synthesize a Branch with Link, precede the LDR instruction with 
 *   MOV LR, PC.
 */

static int debug_ldr(void)
{
	unsigned long addr = 0x80004000;
	unsigned long R0 = 0; /* 1st page dirent */
	unsigned long R1 = addr + 1024 * 8; /* 1st kernel page dirent */
	unsigned long R2 = addr + 1025 * 8; /* 2nd kernel page dirent */
	unsigned long R0_val, R1_val, R2_val;

	/* 
	 * Load memory into Register 
	 *   LDR Rn, [R1] --> Load R1 memory value into Rn register.
	 */
	__asm__ volatile ("ldr %0, [%1]" : "=r" (R1_val) : "r" (R1));
	__asm__ volatile ("ldr %0, [%1]" : "=r" (R2_val) : "r" (R2));

	printf("Memory[%#lx] %#lx\n", R1, R1_val);
	printf("Memory[%#lx] %#lx\n", R2, R2_val);

	/*
	 * Load R1+R2 memory to Register
	 *   LDR Rn, [R1, R2] --> Load memory value into Rn register that 
	 *                        memory address is (R1 + R2).
	 */
	__asm__ volatile ("ldr %0, [%1, %2]" 
			: "=r" (R0_val) : "r" (R0), "r" (R1));
	printf("Memory[%#lx + %#lx] %#lx\n", R0, R1, R0_val);

	/*
	 * Load R1+#1 memory to Register
	 *   LDR Rn, [R1, #8] --> Load memory value into Rn register that
	 *                        memory address is (R1 + #8)
	 */
	__asm__ volatile ("ldr %0, [%1, #8]"
			: "=r" (R1_val) : "r" (R1));
	printf("Memory[%#lx + 8] %#lx\n", R1, R1_val);

	/*
	 * Load R1+R2 memory to Register and load R1+R2 address into R1.
	 *   LDR Rn, [R1 + R2]! --> Load memory value into Rn Reister that
	 *                         memory address is (R1 + R2), and move
	 *                         (R1 + R2) address into R1.
	 */
	R0 = 0;
	__asm__ volatile ("ldr %0, [%1, %2]!"
			: "=r" (R1_val) : "r" (R0), "r" (R1));
	printf("Memory[0 + %#lx] %#lx --> R0: %#lx\n", R1, R1_val, R0);

	/* 
	 * Load R0+#0 memory to Register and load R0+#0 address into R0.
	 *   LDR Rn, [R1 + #0]! --> Load memory value into Rn Register that
	 *                          memory address is  (R1 + #0), and move
	 *                          (R0 + #0) address into R0.
	 */
	R0 = 0x80006000;
	__asm__ volatile ("ldr %0, [%1, #8]!"
			: "=r" (R0_val) : "r" (R0));
	printf("Memory[8 + 0x80006000] %#lx --> R0: %#lx\n", R0_val, R0);


	/*
	 * Load R1 memory to Register and load R1+R2 address into R1.
	 *   LDR Rn, [R1], R2 --> Load R1 memory value into Rn Register and
	 *                        move (R1 + R2) address into R1.
	 */
	R0 = 0x80006000;
	R1 = 8;
	__asm__ volatile ("ldr %0, [%1], %2" : "=r" (R0_val) 
					     : "r" (R0), "r" (R1));
	printf("Memory[0x80006000] %#lx --> R0: %#lx\n", R0_val, R0);

	/*
	 * Load (R1+R2<<N) memory to Register and move (R1+R2<<N) into R1
	 *   LDR Rn, [R1, R2, LSL #3]!
	 */
	R0 = 0x80006000;
	R1 = 1;
	__asm__ volatile ("ldr %0, [%1, %2, lsl #3]!" : "=r" (R0_val)
					: "r" (R0), "r" (R1));
	printf("Memory[0x80006000 + %#lx << 3]: %#lx --> R0: %#lx\n",
			R1, R0_val, R0);

	return 0;
}

/*
 * ldrex
 *
 * (C) 2019.05.05 <buddy.zhang@aliyun.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* Memory access
 *
 *
 *      +----------+
 *      |          |
 *      | Register |                                         +--------+
 *      |          |                                         |        |
 *      +----------+                                         |        |
 *            A                                              |        |
 *            |                                              |        |
 * +-----+    |      +----------+        +----------+        |        |
 * |     |<---o      |          |        |          |        |        |
 * | CPU |<--------->| L1 Cache |<------>| L2 Cache |<------>| Memory |
 * |     |<---o      |          |        |          |        |        |
 * +-----+    |      +----------+        +----------+        |        |
 *            |                                              |        |
 *            o--------------------------------------------->|        |
 *                         volatile/atomic                   |        |
 *                                                           |        |
 *                                                           +--------+
 */

static int ldrex_demo_init(void)
{
static volatile unsigned long R0 = 0;
	unsigned long tmp;
	int result;

	__asm__ volatile ("nop\n\t"
			"ldrex	%0, [%3]\n\t"
			"add	%0, %0, #9\n\t"
			"strex	%1, %0, [%3]"
			: "=&r" (result), "=&r" (tmp), "+Qo" (R0)
			: "r" (&R0)
			: "cc");
	
	printf("R0: %#lx - result: %d - tmp: %#lx\n", R0, result, tmp);

	return 0;
}


/*
 * Logical Shift Left
 *
 * Syntax
 * 
 * RX, LSL #n 
 * or
 * RX, LSL Rn
 */

static int debug_lsl(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 0x1;

	/* MOV R0, R1, LSL #3 --> R0 = R1 << 3 */
	__asm__ volatile ("mov %0, %1, lsl #3"
		: "=r" (a0) : "r" (a1));

	printf("%#lx << 3 = %#lx\n", a1, a0);

	return 0;
}


/*
 * LSR Logical Shift right
 *
 * Syntax
 * 
 * RX, LSR #n 
 * or
 * RX, LSR Rn
 */

int debug_lsr(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 0x8;

	/* MOV R0, R1, LSR #3 --> R0 = R1 >> 3 */
	__asm__ volatile ("mov %0, %1, lsr #3"
		: "=r" (a0) : "r" (a1));

	printf("%#lx >> 3 = %#lx\n", a1, a0);

	return 0;
}


/*
 * STMDA (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmda(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIA: Store Register into memory, and Decrement After
	 *
	 *
	 *             +-------------+
	 *             |             |<-------- R1
	 *             +-------------+
	 *             |             |<-------- R2
	 *             +-------------+
	 *             |             |<-------- R3
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into stack.
	 */
	__asm__ volatile ("stmda %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}
/*
 * STMDB (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmdb(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIA: Store Register into memory, and Increment before
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |<---------- R1
	 *             +-------------+
	 *             |             |<---------- R2
	 *             +-------------+
	 *             |             |<---------- R3
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into stack.
	 */
	__asm__ volatile ("stmdb %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}


/*
 * STMEA (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmea(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIA: Store Register into memory, and empty ascending stack
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |             |<----------- R1
	 *             +-------------+
	 *             |             |<----------- R2
	 *             +-------------+
	 *             |             |<----------- R3
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into empty ascending stack.
	 */
	__asm__ volatile ("stmea %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}


/*
 * STMED (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmed(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIA: Store Register into memory, and empty descending stack
	 *
	 *
	 *             +-------------+
	 *             |             |<---------- R1
	 *             +-------------+
	 *             |             |<---------- R2
	 *             +-------------+
	 *             |             |<---------- R3
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into empty descending stack.
	 */
	__asm__ volatile ("stmed %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}


/*
 * STMFA (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmfa(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIA: Store Register into stack, and Full Ascending stack
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |<--------- R1
	 *    R0[5]--> +-------------+
	 *             |             |<--------- R2
	 *             +-------------+
	 *             |             |<--------- R3
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into full ascending stack.
	 */
	__asm__ volatile ("stmfa %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}


/*
 * STMFD (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmfd(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIA: Store Register into memory, and full descending stack
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |<--------- R1
	 *             +-------------+
	 *             |             |<--------- R2
	 *             +-------------+
	 *             |             |<--------- R3
	 *    R0[5]--> +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into fill descending stack.
	 */
	__asm__ volatile ("stmfd %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}


/*
 * STMIA (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmia(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIA: Store Register into memory, and Increment after
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[5]--> +-------------+
	 *             |             |<------ R1
	 *             +-------------+
	 *             |             |<------ R2
	 *             +-------------+
	 *             |             |<------ R3
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into stack.
	 */
	__asm__ volatile ("stmia %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}


/*
 * STMIB (Store Multiple) stores a non-empty subset (or possibly all) 
 * of the general-purpose registers to sequential memory locations.
 *
 * Syntax
 *   STM{<cond>}<addressing_mode> <Rn>{!}, <registers>
 */


static int debug_stmib(void)
{
static unsigned long R0[10];
	unsigned long R1 = 0x11;
	unsigned long R2 = 0x22;
	unsigned long R3 = 0x33;
	int i;
	
	/*
	 * STMIB: Store Register into memory, and Increment Before
	 *
	 *
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |<------ R1
	 *    R0[5]--> +-------------+
	 *             |             |<------ R2
	 *             +-------------+
	 *             |             |<------ R3
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *             +-------------+
	 *             |             |
	 *    R0[0]--> +-------------+
	 *
	 * Push register into stack.
	 */
	__asm__ volatile ("stmib %0!, {%3, %2, %1}"
			:: "r" (&R0[5]), "r" (R1), "r" (R2), "r" (R3));

	/* Emulate Stack */
	for (i = 0; i < 10; i++)
		printf("R0[%d] %#lx\n", i, R0[i]);

	return 0;
}


/*
 * STR (Store Register) stores a word from a register to memory.
 *
 * Syntax
 *   STR{<cond>} <Rd>, <addressing_mode>
 *
 * Usage
 *
 * - Combined with a suitable addressing mode, STR stores 32-bit data 
 *   from a general-purpose register into memory. Using the PC as the
 *   base register allows PC-relative addressing, which facilitates 
 *   position-independent code.
 */

static int debug_str(void)
{
	pgd_t *pgd = swapper_pg_dir;
	unsigned long R0 = 0x55AA;
	unsigned long R1 = (unsigned long)pgd;
	unsigned long R2;
	
	/* 
	 * Store vlaue into memory 
	 *   STR Rn, [R1] --> Store Rn value into R1 memory.
	 */
	__asm__ volatile ("str %0, [%1]" :: "r" (R0), "r" (R1));
	printf("Store %#lx into %#lx\n", (unsigned long)pgd_val(pgd[0]), R1);

	/*
	 * Store value into memory with index
	 *   STR Rn, [R1, R2] --> Store Rn value into (R1+R2) memory.
	 */
	R2 = 0x8;
	__asm__ volatile ("str %0, [%1, %2]" :: "r" (R0), "r" (R1), "r" (R2));
	printf("Store %#lx into (%#lx+%#lx)\n", 
			(unsigned long)pgd_val(pgd[1]), R1, R2);
	

	/*
	 * Store value into memory with index, and update dest-address
	 *   LDR Rn, [R1, R2]! --> Store Rn value into (R1+R2) memory, and
	 *                         update R1 address as (R1+R2).
	 */
	R2 = 0x16;
	__asm__ volatile ("str %0, [%1, %2]!" :: "r" (R0), "r" (R1), "r" (R2));
	printf("Store %#lx into (%#lx+%#lx) --> R1: %#lx\n",
			(unsigned long)pgd_val(pgd[3]), 
			(unsigned long)pgd, R2, R1);

	/*
	 * Store value into memory and update dest-address.
	 *   LDR Rn, [R1], R2 --> Store Rn value into R1, and update R1
	 *                        address as (R1 + R2)
	 */
	R2 = 0x1;
	__asm__ volatile ("str %0, [%1], %2" :: "r" (R0), "r" (R1), "r" (R1));
	printf("Store %#lx into %#lx --> R1: %#lx\n", R0, 
			(unsigned long)pgd, R1);

	return 0;
}

/*
 * strex
 *
 * (C) 2019.05.05 <buddy.zhang@aliyun.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* Memory access
 *
 *
 *      +----------+
 *      |          |
 *      | Register |                                         +--------+
 *      |          |                                         |        |
 *      +----------+                                         |        |
 *            A                                              |        |
 *            |                                              |        |
 * +-----+    |      +----------+        +----------+        |        |
 * |     |<---o      |          |        |          |        |        |
 * | CPU |<--------->| L1 Cache |<------>| L2 Cache |<------>| Memory |
 * |     |<---o      |          |        |          |        |        |
 * +-----+    |      +----------+        +----------+        |        |
 *            |                                              |        |
 *            o--------------------------------------------->|        |
 *                         volatile/atomic                   |        |
 *                                                           |        |
 *                                                           +--------+
 */


static __init int strex_demo_init(void)
{
static volatile unsigned long R0 = 0;
	unsigned long tmp;
	int result;
	__asm__ volatile ("nop\n\t"
			"ldrex	%0, [%3]\n\t"
			"add	%0, %0, #9\n\t"
			"strex	%1, %0, [%3]"
			: "=&r" (result), "=&r" (tmp), "+Qo" (R0)
			: "r" (&R0)
			: "cc");
	
	printf("R0: %#lx - result: %d - tmp: %#lx\n", R0, result, tmp);

	return 0;
}


/*
 * SUB (Subtract) subtracts one value from a second value. The second value
 * comes from a register. The first value can be either an immediate value
 * or a value from a register, and can be shifted before the subtraction.
 * SUB can optionally update the condition code flags, based on the result.
 *
 * Syntax
 *   SUB{<cond>} {S} <Rd>, <Rn>, <shifter_operand>
 *
 * Usage
 *
 * - Use SUB to subtract one value from another. To decrement a register
 *   value (in Ri ) use:
 *       SUB Ri, Ri, #1
 * - SUBS is useful as a loop counter decrement, as the loop branch can 
 *   test the flags for the appropriate termination condition, without the
 *   need for a separate compare instruction:
 *       SUBS Ri, Ri, #1
 * - This both decrements the loop counter in Ri and checks whether it 
 *   has reached zero.
 * - You can use SUB , with the PC as its destination register and the S 
 *   bit set, to return from interrupts and various other types of 
 *   exception. See Exceptions on page A2-16 for more details.
 */

static int debug_sub(void)
{
	unsigned long a0 = 0;
	unsigned long a1 = 0x8;
	unsigned long a2 = 0x1;

	/* SUB R0, R1, R2 --> R0 = R1 - R2*/
	__asm__ volatile ("sub %0, %1, %2"
		: "=r" (a0) : "r" (a1), "r" (a2));

	printf("%#lx - %#lx = %#lx\n", a1, a2, a0);

	/* SUB R0, R1, R2, LSL #3 --> R0 = R1 - (R2 << 3) */
	__asm__ volatile ("sub %0, %1, %2, lsl #3"
		: "=r" (a0) : "r" (a1), "r" (a2));

	printf("%#lx - (%#lx << 3) = %#lx\n", a1, a2, a0);

	return 0;
}


/*
 * SWP (Swap) swaps a word between registers and memory. SWP loads a word
 * from the memory address given by the value of register <Rn> . The value
 * of register <Rm> is then stored to the memory address given by the 
 * value of <Rn> , and the original loaded value is written to register 
 * <Rd> . If the same register is specified for <Rd> and <Rm> , this 
 * instruction swaps the value of the register and the value at the memory
 * address.
 *
 * Syntax
 *   SWP{<cond>} <Rd>, <Rm>, [<Rn>]
 */


static int debug_swp(void)
{
static unsigned long R2[10] = {0x99};
	unsigned long R1 = 0x88;

	/* SWP R1, R1, [R2] ----> Exchange R2 and R1 value. */
	__asm__ volatile ("mov r1, %1\n\r"
			  "swp r1, r1, [%2]\n\r"
			  "mov r1, %0"
			: "=r" (R1) : "r" (R1), "r" (R2));
	printf("R1: %#lx R2[0]: %#lx\n", R1, R2[0]);

	/* SAP R1, R2, [R0] ---> Load R0 value into R1 */
	__asm__ volatile ("swp r1, r2, [%1]\n\r"
			  "mov r1, %0"
			: "=r" (R1) : "r" (R2));
	printf("R1: %#lx\n", R1);

	return 0;
}