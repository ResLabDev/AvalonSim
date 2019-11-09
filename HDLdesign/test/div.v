module div 
	#(
		parameter W=32,
					 CBIT=6		// CBIT=ld(W)+1
	 )
	(
		input wire clk, reset, str_trg,
		input wire [W-1:0] dvsr, dvnd,
		output reg ready, done_trg,
		output wire [W-1:0] quo,rmd
	
	);
	
	//Symbolic state declaration
	localparam [1:0]
		idle = 2'b00,
		op = 2'b01,
		last = 2'b10,
		done = 2'b11;
		
	//Signal declaration
	reg [1:0] state_reg, state_next;
	reg [W-1:0] rh_reg, rh_next, rl_reg, rl_next, rh_tmp, d_reg, d_next;
	reg [CBIT-1:0] n_reg, n_next;
	reg q_bit, start;
	
	initial
	begin
		start = 1'b0;
	end
	//=======
	//BODY
	//=======
	
	//FSMD state and data registers
	always @ (posedge clk, posedge reset)
		if (reset)
		begin
			state_reg<=idle;
			rh_reg<=0;
			rl_reg<=0;
			d_reg<=0;
			n_reg<=0;
		end
		else
		begin
			state_reg<=state_next;
			rh_reg<=rh_next;
			rl_reg<=rl_next;
			d_reg<=d_next;
			n_reg<=n_next;
		end
		
		//Next-state logic
		always @*
		begin
			if (str_trg)
				start = 1'b1;
			state_next = state_reg;
			ready = 1'b0;
			done_trg = 1'b0;
			rh_next = rh_reg;
			rl_next = rl_reg;
			d_next = d_reg;
			n_next = n_reg;
			case (state_reg)
				idle:
				begin
					ready = 1'b1;
					if (start)
					begin
						rh_next = 0;
						rl_next = dvnd;  //dividend
						d_next = dvsr;   //divisor
						n_next = W+1;    //index
						state_next = op;
					end
				end
				op:
				begin
					// shift rh and rl left
					rl_next  = {rl_reg[W-2:0], q_bit};
					rh_next = {rh_tmp[W-2:0], rl_reg[W-1]};
					// decrease index
					n_next = n_reg-1;
					if (n_next==1)
						state_next = last;
				end
				last: //last iteration
				begin
					rl_next = {rl_reg[W-2:0], q_bit};
					rh_next = rh_tmp;
					state_next = done;
				end
				done:
				begin
					done_trg = 1'b1;
					state_next = idle;
					start = 1'b0;
				end
				default: state_next = idle;
			endcase
		end
		
		//Compare and subtract circuit
		always @*
			if (rh_reg >= d_reg)
			begin
				rh_tmp = rh_reg - d_reg;
				q_bit=1'b1;
			end
			else
			begin
				rh_tmp = rh_reg;
				q_bit = 1'b0;
			end
			
		//Ouput
		assign quo = rl_reg;
		assign rmd = rh_reg;
endmodule








