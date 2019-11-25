`timescale 1ns/1ns
`include "test/avsim_define.v"

module avalon_interface;

 	localparam
		// Avalon bus size
		ADDRESS_SIZE        = 32,  	  // 1-64
		DATA_SIZE           = 32,  		 // 8, 16, 32, 64, 128, 256, 512, 1024 for readdata and writedata
		// Instruction table size
		OPCODE_SIZE         = 4, 				 // Operation code
		INSTR_SIZE          = 68,     // opcode|address|data -> 4|32|32
		INSTR_LIMIT_SIZE    = 7; 				 // Maximum number of acceptable instruction: 2^INSTR_LIMIT_SIZE
		
	reg clk, reset;
	wire avalonMM_chipselect, avalonMM_read, avalonMM_write;
	wire [ADDRESS_SIZE-1:0] avalonMM_address;
	wire [DATA_SIZE-1:0] avalonMM_readdata, readdata;
	wire [DATA_SIZE-1:0] avalonMM_writedata;
    wire avalonMM_irq;
    reg [INSTR_SIZE-1:0] instructionTable [0:(2**INSTR_LIMIT_SIZE)-1];
    wire [INSTR_LIMIT_SIZE-1:0] programCounter;
    wire simReady;
  
	// Instantiate AvalonMM controller module
	avalon_master #(.ADDRESS_SIZE(ADDRESS_SIZE), .DATA_SIZE(DATA_SIZE),
                    .OPCODE_SIZE(OPCODE_SIZE), .INSTR_SIZE(INSTR_SIZE),
                    .INSTR_LIMIT_SIZE(INSTR_LIMIT_SIZE))
    avalonMasterInst
	( 
		// Clock-reset
		.clk(clk), 
		.reset(reset),
		// Avalon MM Master Interface
		.avmaster_chipselect(avalonMM_chipselect),
		.avmaster_read(avalonMM_read),
		.avmaster_write(avalonMM_write),
		.avmaster_address(avalonMM_address),
		.avmaster_readdata(readdata),
		.avmaster_writedata(avalonMM_writedata),
        // Avalon Master Watch
        .readdataWatch(avalonMM_readdata),
        // Instruction I/O
        .programCounter(programCounter),
        .instructionVector(instructionTable[programCounter]),
        // Status
        .simReady(simReady)
	);
		
	// Clock source
	always #10 clk = ~clk;				// 50 MHz
    
	initial begin
	    $readmemh(`INSTRUCTION_PATH, instructionTable); // Store insctruction in the register table
		clk = 1'b0;
		reset = 1'b1;
		#20
		reset = 1'b0;
	end
  
	//========================================================
	// AvalonMM Interface UUT Instantiation
	//========================================================
    
    // --- Integer Devider by Example ---
    wire rdy;
    
    div_avalon #(.W(DATA_SIZE), .CBIT(6)) divAvalonInst1
	(
		// To be connected to Avalon clock  input interface
		.clk(clk),
        .reset(reset),
		// To be connected to Avalon MM slave
		.div_address(avalonMM_address[2:0]),
		.div_chipselect(avalonMM_chipselect),
		.div_write(avalonMM_write),
		.div_writedata(avalonMM_writedata),
		.div_readdata(readdata),
		// To be connected to IS sender interface
		.div_irq(avalonMM_irq),
		// Conduit circuit
		.div_rdy(rdy)
	);
    
    //========================================================
	// Unit Testing
	//========================================================
	integer dividend;
    integer divisor;
    
    task testQuotient;
        input integer result;
        integer expected;
        begin
            expected = dividend / divisor;
            // Validate parameter
           	if (result == expected) begin
                $display("VALID Quotient => %x / %x = %x", dividend, divisor, result);
            end
            else begin
                $display("INVALID Quotient => expected: %x / %x = %x; result: %x",  dividend, divisor, expected, result);
            end
        end
	endtask
	
    task testReminder;
        input integer result;
        integer expected;
        begin
            expected = dividend % divisor;
            // Validate parameter
           	if (result == expected) begin
                $display("VALID Reminder => %x / %x = %x", dividend, divisor, result);
            end
            else begin
                $display("INVALID Reminder => expected: %x / %x = %x; result: %x",  dividend, divisor, expected, result);
            end
        end
	endtask
    
    initial begin
        // Get the dividend and divisor
        wait ((avalonMM_address[2:0] == 3'h0) && avalonMM_write);   // Wait for dividend
        #1
        dividend = avalonMM_writedata;
        wait ((avalonMM_address[2:0] == 3'h1) && avalonMM_write);   // Wait for divisor
        #1
        divisor = avalonMM_writedata;
        // Test the results (quotient and reminder)
        wait ((avalonMM_address[2:0] == 3'h3) && avalonMM_read);    // Wait for quotient
        #21     // ReadWait = 1
        testQuotient(avalonMM_readdata);
        wait ((avalonMM_address[2:0] == 3'h4) && avalonMM_read);    // Wait for reminder
        #21     // ReadWait = 1
        testReminder(avalonMM_readdata);
    end
   
endmodule
