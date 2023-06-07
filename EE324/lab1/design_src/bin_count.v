`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/31/2020 01:09:06 PM
// Design Name: 
// Module Name: bin_count
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


module bin_count #(parameter MAX_COUNT = 255, WIDTH = 8)
(
	input rst, clk, cen,
	output [WIDTH-1:0] val
);
    reg [WIDTH-1:0] counter;
    
    assign val = counter;
    
	always @(posedge(clk)) begin
	   if (rst || val == MAX_COUNT) begin
	       // sync reset
	       counter <= 0;
	   end
	   else if (cen) begin
	       counter <= val + 1;
	   end
	end
endmodule