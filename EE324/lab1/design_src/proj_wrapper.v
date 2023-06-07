`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/31/2020 04:47:29 PM
// Design Name: 
// Module Name: proj_wrapper
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


module proj_wrapper(
        input clk,
        output A, B
    );
    
    wire clkslow; //, wiz_lock;
    
    clk_wiz_0 inst_clk_wiz
	(
        // Clock out ports
        .clk_out1(clkslow),     // output clk_out1
        // Status and control signals
        .reset(1'b0), // input reset
        //.locked(wiz_lock),       // output locked
        // Clock in ports
        .clk_in1(clk)		//input clk_in1
	); 
	
	count_wrap count
	(
	   .clk(clkslow),
	   .A(A),
	   .B(B),
	   .rst(1'b0)
	);

endmodule
