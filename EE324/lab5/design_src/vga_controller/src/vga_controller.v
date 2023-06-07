`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/08/2020 05:10:11 PM
// Design Name: 
// Module Name: vga_controller
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


module vga_controller(
        input clk, rst,
        output hsync, vsync, video_active,
        //output [9:0] hcount, vcount
        output [9:0] x, y,
        output video_enable
    );
    wire ven;
    wire [9:0] hcount, vcount;
    
	bin_count #(
		.MAX_COUNT(800),
		.WIDTH(10) 
	)
	hcounter (
		.rst(rst),
		.clk(clk),
		.cen(1'b1),
		.val(hcount)
	);

	bin_count #(
		.MAX_COUNT(525),
		.WIDTH(10) 
	)
	vcounter (
		.rst(rst),
		.clk(clk),
		.cen(ven),
		.val(vcount)
	);

    assign ven   = hcount == 800;
    assign hsync = (hcount < 96);
    assign vsync = (vcount < 2);
    assign video_enable = ~rst;
    
    assign video_active = (hcount >= (96 + 48))
        && (hcount < (800 - 16))
        && (vcount >= (2 + 33))
        && (vcount < (525 - 10));
        
    assign x = hcount - (96 + 48);
    assign y = vcount - (2 + 33);
endmodule
