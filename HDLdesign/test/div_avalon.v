//========================================
// Wrapping circuit to Avalon @ division
//========================================
// address 
//    0x00: 32-bit dividend (cpu write)
//    0x01: 32-bit divisor (cpu write)
//    0x02: start operation (cpu write)  
//    0x03: 32-bit quotient (cpu read)
//    0x04: 32-bit remainder (cpu read)
//    0x05: bit 0: 1-bit ready (cpu read)  
//    0x06: bit 0: 1-bit done_trg (cpu read/write)
module div_avalon
	#(
		parameter W=32,
					 CBIT=6		// CBIT=ld(W)+1
	 )
	(
		// To be connected to Avalon clock  input interface
		input wire clk, reset,
		// To be connected to Avalon MM slave
		input wire [2:0] div_address,
		input wire div_chipselect,
		input wire div_write,
		input wire [W-1:0] div_writedata,
		output wire [W-1:0] div_readdata,
		// To be connected to IS sender interface
		output wire div_irq,
		// Conduit circuit
		output wire div_rdy
	);
	
	// Signal declaration
	wire div_start, div_ready, set_done_trg, clr_done_trg;
	reg [W-1:0] dvnd_reg, dvsr_reg;
	reg done_trg_reg;
	wire [W-1:0] quo, rmd;
	wire wr_en, wr_dvnd, wr_dvsr;
	
	// Instantiate division circuit
	div #(.W(32), .CBIT(6)) d1
		 ( .clk(clk), .reset(reset), .str_trg(div_start), .dvsr(dvsr_reg), .dvnd(dvnd_reg),  // Input
		   .ready(div_ready), .done_trg(set_done_trg), .quo(quo), .rmd(rmd));        // Output
	
	// Registers
	always @ (posedge clk, posedge reset)
	begin
		if (reset)
		begin
			dvnd_reg <= 0;
			dvsr_reg <= 0;
			done_trg_reg <= 0;
		end
		else
		begin
			if (wr_dvnd)
				dvnd_reg <= div_writedata;
			if (wr_dvsr)
				dvsr_reg <= div_writedata;
			if (set_done_trg)
				done_trg_reg <= 1'b1;
			else if (clr_done_trg)
				done_trg_reg <= 1'b0;
		end
	end
	
	// Write decoding logic
	assign wr_en = div_write & div_chipselect;
	assign wr_dvnd = (div_address == 3'b000) & wr_en;
	assign wr_dvsr = (div_address == 3'b001) & wr_en;
	assign div_start = (div_address == 3'b010) & wr_en;
	assign clr_done_trg = (div_address == 3'b110) & wr_en;
	
	// Read multiplexing logic
	assign div_readdata = (div_address == 3'b011) ? quo :
								 (div_address == 3'b100) ? rmd :
								 (div_address == 3'b101) ? {31'b0, div_ready} : {31'b0, done_trg_reg};
	
	//Conduit circuit: for demonstration only
	assign div_rdy = div_ready;
	
	// Interrupt signals
	assign div_irq = done_trg_reg;
	
endmodule
