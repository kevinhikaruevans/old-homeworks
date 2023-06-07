`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/01/2020 03:43:46 PM
// Design Name: 
// Module Name: vga_syncgen
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


module vga_syncgen(
        input clk, rst,
        output hsync, vsync, video_active,
        //output [9:0] hcount, vcount
        output [9:0] x, y
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
    
    assign video_active = (hcount >= (96 + 48))
        && (hcount < (800 - 16))
        && (vcount >= (2 + 33))
        && (vcount < (525 - 10));
        
    assign x = hcount - (96 + 48);
    assign y = vcount - (2 + 33);
    
endmodule
