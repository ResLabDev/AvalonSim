//==============================================
// Avalon MM Master Interface Module
//  for MM Slave I/O interfacing
//  v2.0
//==============================================
/*
  Instruction format: 1|8|8 hexadecimal --> opcode|address|data
  Operation Codes:
  0 - NOP
  1 - READ
  2 - WRITE
  3 - WAIT
  4 - LOAD
*/
module avalon_master
#( parameter
    // Avalon bus size
    ADDRESS_SIZE        = 32,    // 1-64
    DATA_SIZE           = 32,    // 8, 16, 32, 64, 128, 256, 512, 1024 for readdata and writedata
    // Instruction table size
    OPCODE_SIZE         = 4,     // Operation code
    INSTR_SIZE          = 68,    // opcode|address|data -> 4|32|32
    INSTR_LIMIT_SIZE    = 7     // Maximum number of acceptable instruction: 2^INSTR_LIMIT_SIZE
)
( 
    // Clock-Reset
    input wire clk, 
    input wire reset,
    // Avalon MM Master Interface
    output reg                          avmaster_chipselect,
    output reg                          avmaster_read,
    output reg                          avmaster_write,
    output wire [ADDRESS_SIZE-1:0]      avmaster_address,
    output wire [DATA_SIZE-1:0]         avmaster_writedata,
    input wire [DATA_SIZE-1:0]          avmaster_readdata,
    // Avalon Master Watch
    output wire [DATA_SIZE-1:0]         readdataWatch,
    // Instruction I/O
    output wire [INSTR_LIMIT_SIZE-1:0]  programCounter,
    input wire [INSTR_SIZE-1:0]         instructionVector,
    // Status
    output wire                         simReady
  );
 
// === Constant Definitions ===
    localparam
       AVALON_DELAY       = 25, // Delay of Avalon bus between each operation (measured by analyzator)
       AVALON_PARAM_SIZE   = 8; // Size of Avalon parameters: 2 x hexa = 256
  
    // State register operations (FSM)
    localparam [2:0]
       ST_FETCH         = 3'h0,
       ST_READ_TIMING   = 3'h1,
       ST_READ_LATENCY  = 3'h2,
       ST_WRITE_TIMING  = 3'h3,
       ST_WRITE_HOLD    = 3'h4,
       ST_WAIT          = 3'h5,          
       ST_LOAD          = 3'h6,
       ST_PC_INCR       = 3'h7;

     // Opcode to be FETCHed
     localparam [3:0]
       NOP     = 4'h0, // NOP operation
       READ    = 4'h1, // Read operation
       WRITE   = 4'h2, // Write operation
       WAIT    = 4'h3, // Wait operation
       LOAD    = 4'h4; // LOAD avalon MM slave parameters
     
// === Signal Declarations ===
    // Decoding signals
    wire [3:0]              opCode;  // Operation code
    wire [ADDRESS_SIZE-1:0]    address; // Address line
    wire [DATA_SIZE-1:0]    data;    // Data line
     
    // State register
    reg [2:0] stateNext_reg, state_reg;
     
     // Avalon parameters
    reg [AVALON_PARAM_SIZE-1:0]
        av_setupNext_reg, av_setup_reg, av_setupStore_reg,                      // Avalon setup timing
        av_readWaitNext_reg, av_readWait_reg, av_readWaitStore_reg,             // Avalon readwait timing
        av_writeWaitNext_reg, av_writeWait_reg, av_writeWaitStore_reg,          // Avalon writewait timing
        av_holdNext_reg, av_hold_reg, av_holdStore_reg,                         // Avalon hold timing
        av_readLatencyNext_reg, av_readLatency_reg, av_readLatencyStore_reg;    // Avalon read latency
 
    wire [AVALON_PARAM_SIZE-1:0] setup, readWait, writeWait, hold, readLatency; // Wires for loading parameters
     
    // Waiting sets
    reg [DATA_SIZE-1:0] waitCount_reg, wait_reg, waitNext_reg;  // Wait counter and parameter register
    reg waitCountReset_reg;                                     // Wait counter reset
    wire waitEnd;                                               // Trigger signal
     
    // Internal registers
    reg [7:0] pcNext_reg, pc_reg; // Program counter
     
    // Control registers
    reg readDataEN_reg, loadEN_reg;
     
// === Core Logic ===
     // Wait phase counter
    always @ (posedge clk) begin
        if (waitCountReset_reg) begin
            waitCount_reg <= 0;
        end
        else begin
            waitCount_reg <= waitCount_reg + 1;
        end
    end
         
    // Clock synchronized registers
    always @ (posedge clk, posedge reset) begin
        if (reset) begin
            state_reg <= 0;
            pc_reg <= 0;
            av_setup_reg <= 0;
            av_readWait_reg <= 0;
            av_writeWait_reg <= 0;
            av_hold_reg <= 0;
            av_readLatency_reg <= 0;
            av_setupStore_reg <= 0;
            av_readWaitStore_reg <= 0;
            av_writeWaitStore_reg <= 0;
            av_holdStore_reg <= 0;
            av_readLatencyStore_reg <= 0;    
            wait_reg <= 0;
       end
       else begin
            state_reg <= stateNext_reg;
            pc_reg <= pcNext_reg;
            av_setup_reg <= av_setupNext_reg;
            av_readWait_reg <= av_readWaitNext_reg;
            av_writeWait_reg <= av_writeWaitNext_reg;
            av_hold_reg <= av_holdNext_reg;
            av_readLatency_reg <= av_readLatencyNext_reg;
            wait_reg <= waitNext_reg;
            if (loadEN_reg) begin
                av_setupStore_reg <= setup;
                av_readWaitStore_reg <= readWait;
                av_writeWaitStore_reg <= writeWait;
                av_holdStore_reg <= hold;
                av_readLatencyStore_reg <= readLatency; 
            end
        end
     end
       
     // Finite State Machine
     always @* begin
        // Registers
        stateNext_reg = state_reg;
        pcNext_reg = pc_reg;
        av_setupNext_reg= av_setup_reg;
        av_readWaitNext_reg = av_readWait_reg;
        av_writeWaitNext_reg = av_writeWait_reg;
        av_holdNext_reg = av_hold_reg;
        av_readLatencyNext_reg = av_readLatency_reg;
        waitNext_reg = wait_reg;
        // Default values
        avmaster_chipselect = 1'b0;
        avmaster_read = 1'b0;
        avmaster_write = 1'b0;
        waitCountReset_reg = 1'b1;
        readDataEN_reg = 1'b0;
        loadEN_reg = 1'b0;
        
        case (state_reg)
        //------- Instruction Fetching ---------------
            ST_FETCH: begin
                case (opCode)
                    NOP:  begin                             // No operation
                        stateNext_reg = ST_PC_INCR;         // Next operation
                    end
                    READ: begin                             // Read operation
                      stateNext_reg = ST_READ_TIMING;
                      av_setupNext_reg = av_setupStore_reg;
                      av_readWaitNext_reg = av_readWaitStore_reg;
                      av_readLatencyNext_reg = av_readLatencyStore_reg;
                    end
                    WRITE: begin                            // Write operation
                      stateNext_reg = ST_WRITE_TIMING;
                      av_setupNext_reg = av_setupStore_reg;
                      av_writeWaitNext_reg = av_writeWaitStore_reg;
                      av_holdNext_reg = av_holdStore_reg; 
                    end
                    WAIT: begin                             // Wait operation
                      stateNext_reg = ST_WAIT;
                    end
                    LOAD: begin                             // LOAD avalon MM slave parameters
                      stateNext_reg = ST_LOAD;
                    end
                    default: stateNext_reg = ST_PC_INCR;    // Next instruction
                  endcase // opCode
                // Set avalon wait parameter
                if (opCode == WAIT) begin
                    waitNext_reg = data;
                end
                else begin
                    waitNext_reg = AVALON_DELAY; // basic Avalon bus latency parameter
                end
             end // ST_FETCH
        //-------Read Timing---------------
            ST_READ_TIMING: begin
                avmaster_chipselect = 1'b1;
                if (av_setup_reg) begin
                    av_setupNext_reg = av_setup_reg - 1;
                end
                else if (av_readWait_reg) begin
                    av_readWaitNext_reg = av_readWait_reg - 1;
                    avmaster_read = 1'b1;
                end
                else if (av_readLatency_reg) begin
                    av_readLatencyNext_reg = av_readLatency_reg - 1;
                    stateNext_reg = ST_READ_LATENCY;
                    avmaster_read = 1'b1;
                end
                else begin
                    avmaster_read = 1'b1;
                    stateNext_reg = ST_WAIT;
                    avmaster_read = 1'b1;
                    readDataEN_reg = 1'b1;
                end
             end // ST_READ_TIMING
        //------- Read Latency --------------
            ST_READ_LATENCY: begin
                if (av_readLatency_reg) begin
                    av_readLatencyNext_reg = av_readLatency_reg - 1;
                end
                else begin
                    readDataEN_reg = 1'b1;
                    stateNext_reg = ST_WAIT;
                end
            end // ST_READ_LATENCY
        //------- Write Timing --------------
            ST_WRITE_TIMING: begin
                avmaster_chipselect = 1'b1;
                if (av_setup_reg) begin
                    av_setupNext_reg = av_setup_reg - 1;
                end
                else if (av_writeWait_reg) begin
                    av_writeWaitNext_reg = av_writeWait_reg - 1;
                    avmaster_write = 1'b1;
                end
                else if (av_hold_reg) begin
                    av_holdNext_reg = av_hold_reg - 1;
                    avmaster_write = 1'b1;
                    stateNext_reg = ST_WRITE_HOLD;
                end
                else begin
                    avmaster_write = 1'b1;
                    stateNext_reg = ST_WAIT;
                end
            end // ST_WRITE_TIMING
        //------- Write Hold ----------------
            ST_WRITE_HOLD: begin
                avmaster_chipselect = 1'b1;
                if (av_hold_reg) begin
                    av_holdNext_reg = av_hold_reg - 1;
                end
                else begin
                    stateNext_reg = ST_WAIT;
                end
            end // ST_WRITE_HOLD
        //------- Wait ----------------------
            ST_WAIT: begin
                waitCountReset_reg = 1'b0;
                if (waitEnd) begin
                    stateNext_reg = ST_PC_INCR;
                end
            end // WAIT
        //------- Load ----------------------
            ST_LOAD: begin
              loadEN_reg = 1'b1;
              stateNext_reg = ST_PC_INCR;
            end // ST_LOAD
        //------- Increment Program Counter --------
            ST_PC_INCR: begin
                if (~simReady) begin            // Simulator is not finished
                    pcNext_reg = pc_reg + 1;
                    stateNext_reg = ST_FETCH;
                end
            end // ST_PC_INCR
       endcase // state_reg
         
     end
     
// === Controller Logic ===
     // FETCH instruction table
     assign opCode = instructionVector [INSTR_SIZE-1:INSTR_SIZE-OPCODE_SIZE];
     assign address = instructionVector [INSTR_SIZE-(OPCODE_SIZE+1):INSTR_SIZE-(ADDRESS_SIZE+OPCODE_SIZE)];
     assign data = instructionVector [DATA_SIZE-1:0];
     assign simReady = (instructionVector === {INSTR_SIZE{1'bx}});  // Determine unknown logic with case equality
     
     // Wait state controll signal
     assign waitEnd = (waitCount_reg == (wait_reg-1));
     
     // LOAD parameters
     /* opcode: 4 - LOAD : PARAM = 2xhex = 8bit	
          address|data = 000000(setup)	| (hold, readLatency, writeWait, readWait) */
     assign setup = (state_reg == ST_LOAD) ? address[AVALON_PARAM_SIZE-1:0] : 0;
     assign hold = (state_reg == ST_LOAD) ? data[4*AVALON_PARAM_SIZE-1:3*AVALON_PARAM_SIZE] : 0;
     assign readLatency = (state_reg == ST_LOAD) ? data[3*AVALON_PARAM_SIZE-1:2*AVALON_PARAM_SIZE] : 0;
     assign writeWait = (state_reg == ST_LOAD) ? data[2*AVALON_PARAM_SIZE-1:AVALON_PARAM_SIZE] : 0;
     assign readWait = (state_reg == ST_LOAD) ? data[AVALON_PARAM_SIZE-1:0] : 0;
       
// === Data Path ===     
     // Avalon Bus Data Path
     assign avmaster_address    = (avmaster_chipselect) ? address : 0;
     assign readdataWatch   = (readDataEN_reg) ? avmaster_readdata : 0;
     assign avmaster_writedata  = ((state_reg == ST_WRITE_TIMING) || (state_reg == ST_WRITE_HOLD)) ? data : 0;
     assign programCounter = pc_reg;

endmodule




