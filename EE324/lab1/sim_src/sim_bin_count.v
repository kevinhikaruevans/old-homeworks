`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/28/2020 02:57:26 PM
// Design Name: 
// Module Name: sim_bin_count
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


module sim_bin_count(

    );
    
    reg clk, rst, cen;
    wire[7:0] counter_value;
    
    bin_count counter(
        .clk(clk),
        .rst(rst),
        .cen(cen),
        .val(counter_value)
    );
    
    always
        #5 clk = ~clk;
    initial begin
        clk = 0;
        rst = 1;
        cen = 0;
        
        #10;
        rst = 0;
        
        #10;
        cen = 1;
        
        #30;
        rst = 1;
        
        #10;
        rst = 0;
    end
endmodule
