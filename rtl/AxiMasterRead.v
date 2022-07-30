module AXI4MasterRead (
  output reg          io_dataOut_valid,
  input               io_dataOut_ready,
  output reg [31:0]   io_dataOut_payload,
  input               io_apStart,
  input      [31:0]   io_RAddrOffset,
  input      [31:0]   io_Rlen,
  output              io_apDone,
  output reg          io_axiAR_valid,
  input               io_axiAR_ready,
  output reg [31:0]   io_axiAR_payload_addr,
  output reg [3:0]    io_axiAR_payload_id,
  output reg [3:0]    io_axiAR_payload_region,
  output reg [7:0]    io_axiAR_payload_len,
  output reg [2:0]    io_axiAR_payload_size,
  output reg [1:0]    io_axiAR_payload_burst,
  output reg [0:0]    io_axiAR_payload_lock,
  output reg [3:0]    io_axiAR_payload_cache,
  output reg [3:0]    io_axiAR_payload_qos,
  output reg [0:0]    io_axiAR_payload_user,
  output reg [2:0]    io_axiAR_payload_prot,
  input               io_axiR_valid,
  output              io_axiR_ready,
  input      [31:0]   io_axiR_payload_data,
  input      [3:0]    io_axiR_payload_id,
  input      [1:0]    io_axiR_payload_resp,
  input               io_axiR_payload_last,
  input               clk,
  input               reset
);
  reg                 buffer_1_io_push_valid;
  reg        [31:0]   buffer_1_io_push_payload;
  reg                 buffer_1_io_pop_ready;
  wire                buffer_1_io_push_ready;
  wire                buffer_1_io_pop_valid;
  wire       [31:0]   buffer_1_io_pop_payload;
  wire       [5:0]    buffer_1_io_occupancy;
  wire       [5:0]    buffer_1_io_availability;
  wire       [18:0]   CICC1851_dataCounter_valueNext;
  wire       [0:0]    CICC1851_dataCounter_valueNext_1;
  wire       [31:0]   CICC1851_resdata;
  wire       [31:0]   CICC1851_regArLen;
  wire       [39:0]   CICC1851_regArAddr;
  wire       [31:0]   CICC1851_regArAddr_1;
  wire       [31:0]   CICC1851_regArLen_1;
  reg                 regApStart;
  reg        [31:0]   regRAO;
  reg        [31:0]   regALLRlen;
  reg                 dataCounter_willIncrement;
  wire                dataCounter_willClear;
  reg        [18:0]   dataCounter_valueNext;
  reg        [18:0]   dataCounter_value;
  wire                dataCounter_willOverflowIfInc;
  wire                dataCounter_willOverflow;
  reg        [31:0]   regArAddr;
  reg        [7:0]    regArLen;
  reg                 regConfigFinish;
  reg                 regApDone;
  reg                 firstIn;
  reg                 regReadFinsh;
  wire       [31:0]   resdata;
  wire                __AxiMasterRead_r1;
  wire                __AxiMasterRead_r2;
  wire                __AxiMasterRead_r3;
  wire                __AxiMasterRead_r4;
  wire                __AxiMasterRead_r5;
  wire                __AxiMasterRead_r6;
  wire                __AxiMasterRead_r7;

  assign CICC1851_dataCounter_valueNext_1 = dataCounter_willIncrement;
  assign CICC1851_dataCounter_valueNext = {18'd0, CICC1851_dataCounter_valueNext_1};
  assign CICC1851_resdata = {13'd0, dataCounter_value};
  assign CICC1851_regArLen = (regRAO - 32'h00000001);
  assign CICC1851_regArAddr = ({8'd0,CICC1851_regArAddr_1} <<< 8);
  assign CICC1851_regArAddr_1 = (regArAddr + 32'h00000004);
  assign CICC1851_regArLen_1 = (resdata - 32'h00000001);
  StreamFifo buffer_1 (
    .io_push_valid      (buffer_1_io_push_valid    ), //i
    .io_push_ready      (buffer_1_io_push_ready    ), //o
    .io_push_payload    (buffer_1_io_push_payload  ), //i
    .io_pop_valid       (buffer_1_io_pop_valid     ), //o
    .io_pop_ready       (buffer_1_io_pop_ready     ), //i
    .io_pop_payload     (buffer_1_io_pop_payload   ), //o
    .io_flush           (1'b0                      ), //i
    .io_occupancy       (buffer_1_io_occupancy     ), //o
    .io_availability    (buffer_1_io_availability  ), //o
    .clk                (clk                       ), //i
    .reset              (reset                     )  //i
  );
  always @(*) begin
    dataCounter_willIncrement = 1'b0;
    if(__AxiMasterRead_r5) begin
      dataCounter_willIncrement = 1'b1;
    end
  end

  assign dataCounter_willClear = 1'b0;
  assign dataCounter_willOverflowIfInc = (dataCounter_value == 19'h7e8ff);
  assign dataCounter_willOverflow = (dataCounter_willOverflowIfInc && dataCounter_willIncrement);
  always @(*) begin
    if(dataCounter_willOverflow) begin
      dataCounter_valueNext = 19'h0;
    end else begin
      dataCounter_valueNext = (dataCounter_value + CICC1851_dataCounter_valueNext);
    end
    if(dataCounter_willClear) begin
      dataCounter_valueNext = 19'h0;
    end
  end

  assign resdata = (regALLRlen - CICC1851_resdata);
  assign __AxiMasterRead_r1 = ((regApStart && (! regConfigFinish)) && (! regReadFinsh));
  assign __AxiMasterRead_r2 = (regALLRlen < 32'h00000100);
  assign __AxiMasterRead_r3 = (resdata < 32'h00000100);
  assign __AxiMasterRead_r4 = ((regApStart && regConfigFinish) && (! regReadFinsh));
  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_valid = 1'b1;
    end else begin
      io_axiAR_valid = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_addr = regArAddr;
    end else begin
      io_axiAR_payload_addr = 32'h0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_burst = 2'b01;
    end else begin
      io_axiAR_payload_burst = 2'b00;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_size = 3'b010;
    end else begin
      io_axiAR_payload_size = 3'b000;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_len = regArLen;
    end else begin
      io_axiAR_payload_len = 8'h0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_id = 4'b0000;
    end else begin
      io_axiAR_payload_id = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_lock = 1'b0;
    end else begin
      io_axiAR_payload_lock = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_region = 4'b0000;
    end else begin
      io_axiAR_payload_region = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_cache = 4'b0000;
    end else begin
      io_axiAR_payload_cache = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_qos = 4'b0000;
    end else begin
      io_axiAR_payload_qos = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_user = 1'b0;
    end else begin
      io_axiAR_payload_user = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r4) begin
      io_axiAR_payload_prot = 3'b000;
    end else begin
      io_axiAR_payload_prot = 3'b000;
    end
  end

  assign io_axiR_ready = (buffer_1_io_availability != 6'h0);
  assign __AxiMasterRead_r5 = (io_axiR_valid && io_axiR_ready);
  always @(*) begin
    if(__AxiMasterRead_r5) begin
      buffer_1_io_push_valid = 1'b1;
    end else begin
      buffer_1_io_push_valid = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r5) begin
      buffer_1_io_push_payload = io_axiR_payload_data;
    end else begin
      buffer_1_io_push_payload = 32'h0;
    end
  end

  assign __AxiMasterRead_r6 = (buffer_1_io_occupancy != 6'h0);
  always @(*) begin
    if(__AxiMasterRead_r6) begin
      io_dataOut_valid = buffer_1_io_pop_valid;
    end else begin
      io_dataOut_valid = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r6) begin
      buffer_1_io_pop_ready = io_dataOut_ready;
    end else begin
      buffer_1_io_pop_ready = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterRead_r6) begin
      io_dataOut_payload = buffer_1_io_pop_payload;
    end else begin
      io_dataOut_payload = 32'h0;
    end
  end

  assign __AxiMasterRead_r7 = ((regReadFinsh && (buffer_1_io_occupancy == 6'h01)) && (io_dataOut_valid && io_dataOut_ready));
  assign io_apDone = regApDone;
  always @(posedge clk or posedge reset) begin
    if(reset) begin
      regApStart <= 1'b0;
      regRAO <= 32'h0;
      regALLRlen <= 32'h0;
      dataCounter_value <= 19'h0;
      regArAddr <= io_RAddrOffset;
      regArLen <= 8'h0;
      regConfigFinish <= 1'b0;
      regApDone <= 1'b0;
      firstIn <= 1'b1;
      regReadFinsh <= 1'b0;
    end else begin
      regApStart <= io_apStart;
      regRAO <= io_RAddrOffset;
      regALLRlen <= io_Rlen;
      dataCounter_value <= dataCounter_valueNext;
      if(__AxiMasterRead_r1) begin
        if(__AxiMasterRead_r2) begin
          regArAddr <= regRAO;
          regArLen <= CICC1851_regArLen[7:0];
          firstIn <= 1'b0;
        end else begin
          if(firstIn) begin
            regArAddr <= regRAO;
            firstIn <= 1'b0;
          end else begin
            regArAddr <= CICC1851_regArAddr[31:0];
          end
          if(__AxiMasterRead_r3) begin
            regArLen <= CICC1851_regArLen_1[7:0];
          end else begin
            regArLen <= 8'hff;
          end
        end
        regConfigFinish <= 1'b1;
      end
      if(io_axiR_payload_last) begin
        regConfigFinish <= 1'b0;
      end
      if(dataCounter_willOverflow) begin
        regReadFinsh <= 1'b1;
      end
      if(__AxiMasterRead_r7) begin
        regApDone <= 1'b1;
      end
      if(regApDone) begin
        regConfigFinish <= 1'b0;
        firstIn <= 1'b1;
        regReadFinsh <= 1'b0;
        regApDone <= 1'b0;
      end
    end
  end


endmodule

module StreamFifo (
  input               io_push_valid,
  output              io_push_ready,
  input      [31:0]   io_push_payload,
  output              io_pop_valid,
  input               io_pop_ready,
  output     [31:0]   io_pop_payload,
  input               io_flush,
  output     [5:0]    io_occupancy,
  output     [5:0]    io_availability,
  input               clk,
  input               reset
);
  reg        [31:0]   CICC1851_logic_ram_port0;
  wire       [4:0]    CICC1851_logic_pushPtr_valueNext;
  wire       [0:0]    CICC1851_logic_pushPtr_valueNext_1;
  wire       [4:0]    CICC1851_logic_popPtr_valueNext;
  wire       [0:0]    CICC1851_logic_popPtr_valueNext_1;
  wire                CICC1851_logic_ram_port;
  wire                CICC1851_io_pop_payload;
  wire       [4:0]    CICC1851_io_availability;
  reg                 CICC1851_1;
  reg                 logic_pushPtr_willIncrement;
  reg                 logic_pushPtr_willClear;
  reg        [4:0]    logic_pushPtr_valueNext;
  reg        [4:0]    logic_pushPtr_value;
  wire                logic_pushPtr_willOverflowIfInc;
  wire                logic_pushPtr_willOverflow;
  reg                 logic_popPtr_willIncrement;
  reg                 logic_popPtr_willClear;
  reg        [4:0]    logic_popPtr_valueNext;
  reg        [4:0]    logic_popPtr_value;
  wire                logic_popPtr_willOverflowIfInc;
  wire                logic_popPtr_willOverflow;
  wire                logic_ptrMatch;
  reg                 logic_risingOccupancy;
  wire                logic_pushing;
  wire                logic_popping;
  wire                logic_empty;
  wire                logic_full;
  reg                 CICC1851_io_pop_valid;
  wire                __Stream_l955;
  wire       [4:0]    logic_ptrDif;
  reg [31:0] logic_ram [0:31];

  assign CICC1851_logic_pushPtr_valueNext_1 = logic_pushPtr_willIncrement;
  assign CICC1851_logic_pushPtr_valueNext = {4'd0, CICC1851_logic_pushPtr_valueNext_1};
  assign CICC1851_logic_popPtr_valueNext_1 = logic_popPtr_willIncrement;
  assign CICC1851_logic_popPtr_valueNext = {4'd0, CICC1851_logic_popPtr_valueNext_1};
  assign CICC1851_io_availability = (logic_popPtr_value - logic_pushPtr_value);
  assign CICC1851_io_pop_payload = 1'b1;
  always @(posedge clk) begin
    if(CICC1851_io_pop_payload) begin
      CICC1851_logic_ram_port0 <= logic_ram[logic_popPtr_valueNext];
    end
  end

  always @(posedge clk) begin
    if(CICC1851_1) begin
      logic_ram[logic_pushPtr_value] <= io_push_payload;
    end
  end

  always @(*) begin
    CICC1851_1 = 1'b0;
    if(logic_pushing) begin
      CICC1851_1 = 1'b1;
    end
  end

  always @(*) begin
    logic_pushPtr_willIncrement = 1'b0;
    if(logic_pushing) begin
      logic_pushPtr_willIncrement = 1'b1;
    end
  end

  always @(*) begin
    logic_pushPtr_willClear = 1'b0;
    if(io_flush) begin
      logic_pushPtr_willClear = 1'b1;
    end
  end

  assign logic_pushPtr_willOverflowIfInc = (logic_pushPtr_value == 5'h1f);
  assign logic_pushPtr_willOverflow = (logic_pushPtr_willOverflowIfInc && logic_pushPtr_willIncrement);
  always @(*) begin
    logic_pushPtr_valueNext = (logic_pushPtr_value + CICC1851_logic_pushPtr_valueNext);
    if(logic_pushPtr_willClear) begin
      logic_pushPtr_valueNext = 5'h0;
    end
  end

  always @(*) begin
    logic_popPtr_willIncrement = 1'b0;
    if(logic_popping) begin
      logic_popPtr_willIncrement = 1'b1;
    end
  end

  always @(*) begin
    logic_popPtr_willClear = 1'b0;
    if(io_flush) begin
      logic_popPtr_willClear = 1'b1;
    end
  end

  assign logic_popPtr_willOverflowIfInc = (logic_popPtr_value == 5'h1f);
  assign logic_popPtr_willOverflow = (logic_popPtr_willOverflowIfInc && logic_popPtr_willIncrement);
  always @(*) begin
    logic_popPtr_valueNext = (logic_popPtr_value + CICC1851_logic_popPtr_valueNext);
    if(logic_popPtr_willClear) begin
      logic_popPtr_valueNext = 5'h0;
    end
  end

  assign logic_ptrMatch = (logic_pushPtr_value == logic_popPtr_value);
  assign logic_pushing = (io_push_valid && io_push_ready);
  assign logic_popping = (io_pop_valid && io_pop_ready);
  assign logic_empty = (logic_ptrMatch && (! logic_risingOccupancy));
  assign logic_full = (logic_ptrMatch && logic_risingOccupancy);
  assign io_push_ready = (! logic_full);
  assign io_pop_valid = ((! logic_empty) && (! (CICC1851_io_pop_valid && (! logic_full))));
  assign io_pop_payload = CICC1851_logic_ram_port0;
  assign __Stream_l955 = (logic_pushing != logic_popping);
  assign logic_ptrDif = (logic_pushPtr_value - logic_popPtr_value);
  assign io_occupancy = {(logic_risingOccupancy && logic_ptrMatch),logic_ptrDif};
  assign io_availability = {((! logic_risingOccupancy) && logic_ptrMatch),CICC1851_io_availability};
  always @(posedge clk or posedge reset) begin
    if(reset) begin
      logic_pushPtr_value <= 5'h0;
      logic_popPtr_value <= 5'h0;
      logic_risingOccupancy <= 1'b0;
      CICC1851_io_pop_valid <= 1'b0;
    end else begin
      logic_pushPtr_value <= logic_pushPtr_valueNext;
      logic_popPtr_value <= logic_popPtr_valueNext;
      CICC1851_io_pop_valid <= (logic_popPtr_valueNext == logic_pushPtr_value);
      if(__Stream_l955) begin
        logic_risingOccupancy <= logic_pushing;
      end
      if(io_flush) begin
        logic_risingOccupancy <= 1'b0;
      end
    end
  end


endmodule
