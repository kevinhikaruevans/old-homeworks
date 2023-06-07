
`timescale 1 ns / 1 ps

	module proj3_axi_ip_v1_0 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4
	)
	(
		// Users to add ports here

		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready,
		
		output[3:0] led,
		output[5:0] rgb_led
	);
	
    
    (* mark_debug = "true", keep = "true" *)
	reg [31:0] reg1;
	(* mark_debug = "true", keep = "true" *)
	reg [31:0] reg2;
	reg [31:0] tmp;
	
	(* mark_debug = "true", keep = "true" *)
	wire write_en;
	(* mark_debug = "true", keep = "true" *)
	wire[1:0] wraddr;
	
	(* mark_debug = "true", keep = "true" *)
	wire [31:0] rdata_in;
	
	assign led = reg1[3:0];
	assign rgb_led = reg2[5:0];
	
	//instatiate axi slave logic controller
	axi_write_logic wrinst(

		.axi_clk(s00_axi_aclk),
		.rstn(s00_axi_aresetn),

		.write_addr(s00_axi_awaddr[3:2]),
		.write_addr_valid(s00_axi_awvalid),
		.write_addr_ready(s00_axi_awready),


		.write_data(s00_axi_wdata),
		.write_data_valid(s00_axi_wvalid),
		.write_data_ready(s00_axi_wready),

		.write_resp(s00_axi_bresp),
		.write_resp_ready(s00_axi_bready),
		.write_resp_valid(s00_axi_bvalid),


		.data_out(rdata_in),
		.addr_out(wraddr),
		.data_valid(write_en)
	);
	
	always @(posedge s00_axi_aclk) begin
	   if (s00_axi_aresetn == 0) begin
	       reg1 <= 32'b0;
	       reg2 <= 32'b0;
	   end
	   else if (write_en == 1) begin
	       case (wraddr) 
	           0: reg1 <= rdata_in;
	           1: reg2 <= rdata_in;
	           default:
	               tmp <= rdata_in;
	       endcase
	   end
	end
	
	// read logic
	// read addr handshake:
	
	reg read_addr_ready;
	reg [31:0] read_addr_latch;
	reg [31:0] data_out_latch;
	reg data_out_valid;
	
	reg has_data;
	
	assign s00_axi_arready = read_addr_ready;
	assign s00_axi_rdata = data_out_latch;
	assign s00_axi_rvalid = data_out_valid;
	assign s00_axi_rresp = 2'b0;
	
	always @(posedge s00_axi_aclk) begin
        if (s00_axi_aresetn == 0) begin
            read_addr_ready <= 0;  	       
        end
        else if (s00_axi_arvalid & read_addr_ready) begin
            read_addr_ready <= 0;
        end
        else if (s00_axi_arvalid) begin
            read_addr_ready <= 1;
        end
	end
	
	always @(posedge s00_axi_aclk) begin
	    if (s00_axi_aresetn == 0) begin
	        read_addr_latch <= 32'b0;
	    end
	    else begin
	       if (s00_axi_arvalid & read_addr_ready)
	           read_addr_latch <= s00_axi_araddr;
	    end
	end
	
	always @(posedge s00_axi_aclk) begin
	   if (s00_axi_arvalid & read_addr_ready) begin
	       case (read_addr_latch[3:2])
	           0: data_out_latch <= reg1;
	           1: data_out_latch <= reg2;
	           default:
	               data_out_latch <= 32'hE404;
	       endcase
	   end
	end
	
	
	always @(posedge s00_axi_aclk) begin
        if (s00_axi_aresetn == 0) begin
	        data_out_valid <= 0;
	    end
	    else if (data_out_valid & s00_axi_rready) begin
	       data_out_valid <= 0;
	    end
	    else if (s00_axi_arvalid & read_addr_ready) begin
	        data_out_valid <= 1;   
	    end
	end
	
	
	
//	reg [31:0] read_data;
//	reg [31:0] read_addr;
//	reg axi_arready;
//	reg data_written;
//	reg axi_rvalid;
	
//	assign s00_axi_rdata = read_data;
//	assign s00_axi_arready = axi_arready;
//	assign s00_axi_rvalid = axi_rvalid;
	
//	always @(posedge s00_axi_aclk) begin
//	   if (s00_axi_aresetn == 0) begin
//	       read_addr <= 32'b0;
//	   end
//	   else begin
//	       if (s00_axi_arvalid) begin
//	           read_addr <= s00_axi_araddr;
//	           axi_arready <= 1;
//	       end
//	   end
//	end
	
////	always @(s00_axi_rready, read_addr) begin
////	   if (s00_axi_rready
////	end

//	always @(posedge s00_axi_aclk) begin
//	   if (s00_axi_aresetn == 0) begin
//	       read_data <= 32'b0;
//	   end
//	   else if (s00_axi_rready) begin
//	       case (read_addr) 
//	           1: read_data <= reg1;
//	           2: read_data <= reg2;
//	           default:
//	               read_data <= 32'b1;
//	       endcase
//	   end
//	end

	
//	always @(posedge s00_axi_aclk) begin
//	   if (s00_axi_aresetn == 0) begin
//	       data_written <= 0;
//	   end
//	   else if (data_written) begin
//	       axi_rvalid <= 1;
//	       data_written <= 0;
//	   end if (s00_axi_rready) begin	       
//	       data_written <= 1;
//	   end
//	   else begin
//	       axi_rvalid <= 0;
//	   end
//	end
// Instantiation of Axi Bus Interface S00_AXI
//	proj3_axi_ip_v1_0_S00_AXI # ( 
//		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
//		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
//	) proj3_axi_ip_v1_0_S00_AXI_inst (
//		.S_AXI_ACLK(s00_axi_aclk),
//		.S_AXI_ARESETN(s00_axi_aresetn),
//		.S_AXI_AWADDR(s00_axi_awaddr),
//		.S_AXI_AWPROT(s00_axi_awprot),
//		.S_AXI_AWVALID(s00_axi_awvalid),
//		.S_AXI_AWREADY(s00_axi_awready),
//		.S_AXI_WDATA(s00_axi_wdata),
//		.S_AXI_WSTRB(s00_axi_wstrb),
//		.S_AXI_WVALID(s00_axi_wvalid),
//		.S_AXI_WREADY(s00_axi_wready),
//		.S_AXI_BRESP(s00_axi_bresp),
//		.S_AXI_BVALID(s00_axi_bvalid),
//		.S_AXI_BREADY(s00_axi_bready),
//		.S_AXI_ARADDR(s00_axi_araddr),
//		.S_AXI_ARPROT(s00_axi_arprot),
//		.S_AXI_ARVALID(s00_axi_arvalid),
//		.S_AXI_ARREADY(s00_axi_arready),
//		.S_AXI_RDATA(s00_axi_rdata),
//		.S_AXI_RRESP(s00_axi_rresp),
//		.S_AXI_RVALID(s00_axi_rvalid),
//		.S_AXI_RREADY(s00_axi_rready)
//	);

	// Add user logic here

	// User logic ends

	endmodule
