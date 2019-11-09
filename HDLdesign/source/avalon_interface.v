`timescale 1ns/1ns

module avalon_interface;

 	localparam
		// Avalon bus size
		ADDRESS_SIZE        = 32,  				// 1-64
		DATA_SIZE 	        = 32,  				// 8, 16, 32, 64, 128, 256, 512, 1024 for readdata and writedata
		// Instruction table size
		OPCODE_SIZE         = 4, 				// Operation code
		INSTR_SIZE          = 68,  				// opcode|address|data -> 4|32|32
		INSTR_LIMIT_SIZE    = 7; 				// Maximum number of acceptable instruction: 2^INSTR_LIMIT_SIZE
		
    reg [INSTR_SIZE-1:0] instructionTable [0:(2**INSTR_LIMIT_SIZE)-1];
	reg clk, reset, irc;
	wire chipselect, read, write, status;
	wire [ADDRESS_SIZE-1:0] address;
	wire [DATA_SIZE-1:0] readdataWatch, readdata;
	wire [DATA_SIZE-1:0] writedata;
  
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
		.avmaster_chipselect_o(chipselect),
		.avmaster_read(read),
		.avmaster_write(write),
		.avmaster_address(address),
		.avmaster_readdata(readdata),
		.avmaster_writedata(writedata)
        // Avalon Master Watch
        .avmaster_readdata(readdataWatch),
        // Instruction I/O
        .programCounter(programCounter),
        .instructionVector(programCounter)
	);
	
	// Clock source
	always #10 clk = ~clk;				// 50 MHz
	  // Store insctruction in register table


	initial begin
	    $readmemh("../test/instruction.mem", instructionTable);
		clk = 1'b0;
		reset = 1'b1;
		irc = 0;
		#40
		reset = 1'b0;
		#40
		reset = 1'b0;
		#9000
		irc = 1'b1;
		#1500
		irc = 1'b0;
	end
  
	//========================================================
	// AvalonMM interface module to be tested
	//========================================================

  
endmodule

