module AXI4MasterWrite (
  input               io_apStart,
  input               io_data_in_valid,
  output              io_data_in_ready,
  input      [31:0]   io_data_in_payload,
  input      [31:0]   io_WAddrOffset,
  input      [31:0]   io_Wlen,
  output reg          io_axiAW_valid,
  input               io_axiAW_ready,
  output reg [31:0]   io_axiAW_payload_addr,
  output reg [3:0]    io_axiAW_payload_id,
  output reg [3:0]    io_axiAW_payload_region,
  output reg [7:0]    io_axiAW_payload_len,
  output reg [2:0]    io_axiAW_payload_size,
  output reg [1:0]    io_axiAW_payload_burst,
  output reg [0:0]    io_axiAW_payload_lock,
  output reg [3:0]    io_axiAW_payload_cache,
  output reg [3:0]    io_axiAW_payload_qos,
  output reg [0:0]    io_axiAW_payload_user,
  output reg [2:0]    io_axiAW_payload_prot,
  output              io_axiW_valid,
  input               io_axiW_ready,
  output     [31:0]   io_axiW_payload_data,
  output     [3:0]    io_axiW_payload_strb,
  output              io_axiW_payload_last,
  input               io_axiB_valid,
  output              io_axiB_ready,
  input      [3:0]    io_axiB_payload_id,
  input      [1:0]    io_axiB_payload_resp,
  output              io_apDone,
  input               clk,
  input               reset
);
  reg                 buffer_1_io_push_valid;
  reg        [31:0]   buffer_1_io_push_payload;
  reg                 buffer_1_io_flush;
  wire                buffer_1_io_push_ready;
  wire                buffer_1_io_pop_valid;
  wire       [31:0]   buffer_1_io_pop_payload;
  wire       [5:0]    buffer_1_io_occupancy;
  wire       [5:0]    buffer_1_io_availability;
  wire       [4:0]    CICC1851_dataCounter_valueNext;
  wire       [0:0]    CICC1851_dataCounter_valueNext_1;
  wire       [4:0]    CICC1851_dataInCounter_valueNext;
  wire       [0:0]    CICC1851_dataInCounter_valueNext_1;
  wire       [7:0]    CICC1851_data255flag_valueNext;
  wire       [0:0]    CICC1851_data255flag_valueNext_1;
  wire       [31:0]   CICC1851_resdata;
  wire       [31:0]   CICC1851_regAwLen;
  wire       [39:0]   CICC1851_regAwAddr;
  wire       [31:0]   CICC1851_regAwAddr_1;
  wire       [31:0]   CICC1851_regAwLen_1;
  wire       [31:0]   CICC1851___AxiMasterWrite_w1;
  wire       [31:0]   CICC1851___AxiMasterWrite_w1_1;
  reg                 regApStart;
  reg        [31:0]   regWAO;
  reg        [31:0]   regALLWlen;
  reg                 dataCounter_willIncrement;
  reg                 dataCounter_willClear;
  reg        [4:0]    dataCounter_valueNext;
  reg        [4:0]    dataCounter_value;
  wire                dataCounter_willOverflowIfInc;
  wire                dataCounter_willOverflow;
  reg                 dataInCounter_willIncrement;
  reg                 dataInCounter_willClear;
  reg        [4:0]    dataInCounter_valueNext;
  reg        [4:0]    dataInCounter_value;
  wire                dataInCounter_willOverflowIfInc;
  wire                dataInCounter_willOverflow;
  reg                 data255flag_willIncrement;
  reg                 data255flag_willClear;
  reg        [7:0]    data255flag_valueNext;
  reg        [7:0]    data255flag_value;
  wire                data255flag_willOverflowIfInc;
  wire                data255flag_willOverflow;
  reg        [31:0]   regAwAddr;
  reg        [7:0]    regAwLen;
  reg                 regConfigFinish;
  reg                 regwlast;
  reg                 regApDone;
  wire       [31:0]   resdata;
  reg                 firstIn;
  reg                 regWriteFinish;
  wire                __AxiMasterWrite_w2;
  wire                __AxiMasterWrite_w3;
  wire                __AxiMasterWrite_w4;
  wire                __AxiMasterWrite_w5;
  wire                __AxiMasterWrite_w6;
  wire                __AxiMasterWrite_w7;
  wire                __AxiMasterWrite_w1;
  wire                __AxiMasterWrite_w8;

  assign CICC1851_dataCounter_valueNext_1 = dataCounter_willIncrement;
  assign CICC1851_dataCounter_valueNext = {4'd0, CICC1851_dataCounter_valueNext_1};
  assign CICC1851_dataInCounter_valueNext_1 = dataInCounter_willIncrement;
  assign CICC1851_dataInCounter_valueNext = {4'd0, CICC1851_dataInCounter_valueNext_1};
  assign CICC1851_data255flag_valueNext_1 = data255flag_willIncrement;
  assign CICC1851_data255flag_valueNext = {7'd0, CICC1851_data255flag_valueNext_1};
  assign CICC1851_resdata = {27'd0, dataCounter_value};
  assign CICC1851_regAwLen = (regWAO - 32'h00000001);
  assign CICC1851_regAwAddr = ({8'd0,CICC1851_regAwAddr_1} <<< 8);
  assign CICC1851_regAwAddr_1 = (regAwAddr + 32'h00000004);
  assign CICC1851_regAwLen_1 = (resdata - 32'h00000001);
  assign CICC1851___AxiMasterWrite_w = {27'd0, dataCounter_value};
  assign CICC1851___AxiMasterWrite_w_1 = (io_Wlen - 32'h00000002);
  StreamFifo buffer_1 (
    .io_push_valid      (buffer_1_io_push_valid    ), //i
    .io_push_ready      (buffer_1_io_push_ready    ), //o
    .io_push_payload    (buffer_1_io_push_payload  ), //i
    .io_pop_valid       (buffer_1_io_pop_valid     ), //o
    .io_pop_ready       (io_axiW_ready             ), //i
    .io_pop_payload     (buffer_1_io_pop_payload   ), //o
    .io_flush           (buffer_1_io_flush         ), //i
    .io_occupancy       (buffer_1_io_occupancy     ), //o
    .io_availability    (buffer_1_io_availability  ), //o
    .clk                (clk                       ), //i
    .reset              (reset                     )  //i
  );
  always @(*) begin
    dataCounter_willIncrement = 1'b0;
    if(__AxiMasterWrite_w7) begin
      dataCounter_willIncrement = 1'b1;
    end
  end

  always @(*) begin
    dataCounter_willClear = 1'b0;
    if(dataCounter_willOverflow) begin
      dataCounter_willClear = 1'b1;
    end
  end

  assign dataCounter_willOverflowIfInc = (dataCounter_value == 5'h1f);
  assign dataCounter_willOverflow = (dataCounter_willOverflowIfInc && dataCounter_willIncrement);
  always @(*) begin
    dataCounter_valueNext = (dataCounter_value + CICC1851_dataCounter_valueNext);
    if(dataCounter_willClear) begin
      dataCounter_valueNext = 5'h0;
    end
  end

  always @(*) begin
    dataInCounter_willIncrement = 1'b0;
    if(__AxiMasterWrite_w2) begin
      dataInCounter_willIncrement = 1'b1;
    end
  end

  always @(*) begin
    dataInCounter_willClear = 1'b0;
    if(dataInCounter_willOverflow) begin
      dataInCounter_willClear = 1'b1;
    end
  end

  assign dataInCounter_willOverflowIfInc = (dataInCounter_value == 5'h1f);
  assign dataInCounter_willOverflow = (dataInCounter_willOverflowIfInc && dataInCounter_willIncrement);
  always @(*) begin
    dataInCounter_valueNext = (dataInCounter_value + CICC1851_dataInCounter_valueNext);
    if(dataInCounter_willClear) begin
      dataInCounter_valueNext = 5'h0;
    end
  end

  always @(*) begin
    data255flag_willIncrement = 1'b0;
    if(__AxiMasterWrite_w7) begin
      data255flag_willIncrement = 1'b1;
    end
  end

  always @(*) begin
    data255flag_willClear = 1'b0;
    if(dataCounter_willOverflow) begin
      data255flag_willClear = 1'b1;
    end
  end

  assign data255flag_willOverflowIfInc = (data255flag_value == 8'hff);
  assign data255flag_willOverflow = (data255flag_willOverflowIfInc && data255flag_willIncrement);
  always @(*) begin
    data255flag_valueNext = (data255flag_value + CICC1851_data255flag_valueNext);
    if(data255flag_willClear) begin
      data255flag_valueNext = 8'h0;
    end
  end

  assign io_data_in_ready = (((buffer_1_io_availability != 6'h0) && (! regWriteFinish)) && regApStart);
  assign __AxiMasterWrite_w2 = (io_data_in_valid && io_data_in_ready);
  always @(*) begin
    if(__AxiMasterWrite_w2) begin
      buffer_1_io_push_valid = 1'b1;
    end else begin
      buffer_1_io_push_valid = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w2) begin
      buffer_1_io_push_payload = io_data_in_payload;
    end else begin
      buffer_1_io_push_payload = 32'h0;
    end
  end

  assign resdata = (regALLWlen - CICC1851_resdata);
  assign io_axiB_ready = 1'b1;
  assign __AxiMasterWrite_w3 = (((regApStart && (! regConfigFinish)) && (! regWriteFinish)) && (firstIn || (io_axiB_valid && io_axiB_ready)));
  assign __AxiMasterWrite_w4 = (regALLWlen < 32'h00000100);
  assign __AxiMasterWrite_w5 = (resdata < 32'h00000100);
  assign __AxiMasterWrite_w6 = ((regApStart && (! regApDone)) && regConfigFinish);
  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_valid = 1'b1;
    end else begin
      io_axiAW_valid = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_addr = regAwAddr;
    end else begin
      io_axiAW_payload_addr = 32'h0;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_burst = 2'b01;
    end else begin
      io_axiAW_payload_burst = 2'b00;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_size = 3'b010;
    end else begin
      io_axiAW_payload_size = 3'b000;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_len = regAwLen;
    end else begin
      io_axiAW_payload_len = 8'h0;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_id = 4'b0000;
    end else begin
      io_axiAW_payload_id = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_lock = 1'b0;
    end else begin
      io_axiAW_payload_lock = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_region = 4'b0000;
    end else begin
      io_axiAW_payload_region = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_cache = 4'b0000;
    end else begin
      io_axiAW_payload_cache = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_qos = 4'b0000;
    end else begin
      io_axiAW_payload_qos = 4'b0000;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_user = 1'b0;
    end else begin
      io_axiAW_payload_user = 1'b0;
    end
  end

  always @(*) begin
    if(__AxiMasterWrite_w6) begin
      io_axiAW_payload_prot = 3'b000;
    end else begin
      io_axiAW_payload_prot = 3'b000;
    end
  end

  assign io_axiW_valid = buffer_1_io_pop_valid;
  assign io_axiW_payload_data = buffer_1_io_pop_payload;
  assign io_axiW_payload_last = (regwlast && (io_axiW_valid && io_axiW_ready));
  assign io_axiW_payload_strb = {1'b1,{1'b1,{1'b1,1'b1}}};
  assign io_apDone = regApDone;
  assign __AxiMasterWrite_w7 = (io_axiW_valid && io_axiW_ready);
  always @(*) begin
    buffer_1_io_flush = 1'b0;
    if(regApDone) begin
      buffer_1_io_flush = 1'b1;
    end
  end

  assign __AxiMasterWrite_w = (((dataCounter_value != 5'h0) && ((data255flag_value == 8'hfe) || (CICC1851___AxiMasterWrite_w == CICC1851___AxiMasterWrite_w_1))) && (io_axiW_valid && io_axiW_ready));
  assign __AxiMasterWrite_w8 = (regwlast && (io_axiW_valid && io_axiW_ready));
  always @(posedge clk or posedge reset) begin
    if(reset) begin
      regApStart <= 1'b0;
      regWAO <= 32'h0;
      regALLWlen <= 32'h0;
      dataCounter_value <= 5'h0;
      dataInCounter_value <= 5'h0;
      data255flag_value <= 8'h0;
      regAwAddr <= io_WAddrOffset;
      regAwLen <= 8'h0;
      regConfigFinish <= 1'b0;
      regwlast <= 1'b0;
      regApDone <= 1'b0;
      firstIn <= 1'b1;
      regWriteFinish <= 1'b0;
    end else begin
      regApStart <= io_apStart;
      regWAO <= io_WAddrOffset;
      regALLWlen <= io_Wlen;
      dataCounter_value <= dataCounter_valueNext;
      dataInCounter_value <= dataInCounter_valueNext;
      data255flag_value <= data255flag_valueNext;
      if(__AxiMasterWrite_w3) begin
        if(__AxiMasterWrite_w4) begin
          regAwAddr <= regWAO;
          regAwLen <= CICC1851_regAwLen[7:0];
          firstIn <= 1'b0;
        end else begin
          if(firstIn) begin
            regAwAddr <= regWAO;
            firstIn <= 1'b0;
          end else begin
            regAwAddr <= CICC1851_regAwAddr[31:0];
          end
          if(__AxiMasterWrite_w5) begin
            regAwLen <= CICC1851_regAwLen_1[7:0];
          end else begin
            regAwLen <= 8'hff;
          end
        end
        regConfigFinish <= 1'b1;
      end
      if(data255flag_willOverflow) begin
        regConfigFinish <= 1'b0;
      end
      if(dataCounter_willOverflow) begin
        regApDone <= 1'b1;
      end
      if(dataInCounter_willOverflow) begin
        regWriteFinish <= 1'b1;
      end
      if(__AxiMasterWrite_w) begin
        regwlast <= 1'b1;
      end else begin
        if(__AxiMasterWrite_w8) begin
          regwlast <= 1'b0;
        end
      end
      if(regApDone) begin
        regConfigFinish <= 1'b0;
        firstIn <= 1'b1;
        regApDone <= 1'b0;
        regWriteFinish <= 1'b0;
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