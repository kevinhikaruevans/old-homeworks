`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/01/2020 04:17:58 PM
// Design Name: 
// Module Name: vga_wrapper
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module vga_wrapper(
        input clk,
        input[3:0] btn,
        output hdmi_clk_n, hdmi_clk_p, hdmi_en,
        output [2:0] hdmi_tx_n, hdmi_tx_p
    );
    
    wire vgaclk, vgaclk5x, vgaclk_locked;
    
    assign hdmi_en = 1'b1;
    
    vga_clk_wiz_1 inst_clk_wiz
	(
        // Clock out ports
        .clk_out1(vgaclk),     // output clk_out1
        .clk_out2(vgaclk5x),
        
        // Status and control signals
        .reset(1'b0), // input reset
        .locked(vgaclk_locked),       // output locked
        // Clock in ports
        .clk_in1(clk)		//input clk_in1
	); 
	    
    wire hsync, vsync, vde;
    
    wire [3:0] red, green, blue;
    
	vga_controller vga (
	   .vgaclk(vgaclk),
	   .HS(hsync),
	   .VS(vsync),
	   .vde(vde),
	   
	   .vga_r(red),
	   .vga_g(green),
	   .vga_b(blue),
	   
	   .btn(btn)
	);
	 
    hdmi_tx_0 my_hdmi_inst (
        .pix_clk(vgaclk),
        .pix_clkx5(vgaclk5x),
        .pix_clk_locked(vgaclk_locked),
        .rst(1'b0),
        
        .red(red),
        .green(green),
        .blue(blue),
        
        .hsync(hsync),
        .vsync(vsync),
        .vde(vde),
        
        .aux0_din(0),
        .aux1_din(0),
        .aux2_din(0),
        .ade(0),
        
        .TMDS_CLK_P(hdmi_clk_p),
        .TMDS_CLK_N(hdmi_clk_n),
        
        .TMDS_DATA_P(hdmi_tx_p),
        .TMDS_DATA_N(hdmi_tx_n)
    );
endmodule
