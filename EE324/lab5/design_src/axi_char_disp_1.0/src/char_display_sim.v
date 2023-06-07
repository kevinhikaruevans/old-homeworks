`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/15/2020 09:58:49 AM
// Design Name: 
// Module Name: char_display_sim
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


module char_display_sim(

    );
    
    /* 
module char_display(
        input vga_clk,
        input[9:0] vga_x, vga_y,
        input[7:0] ascii_value,
        input[9:0] target_x, target_y,
        output on
    );
     */
     
    reg vga_clk = 1'b1;
    reg [9:0] vga_x = 0;
    reg [9:0] vga_y = 0;
    wire on;
    
    always
    begin
        #5 vga_clk <= ~vga_clk;
    end

    always @(posedge(vga_clk))
    begin
        if (vga_x < 30)
            vga_x <= vga_x + 1;
        else begin
            vga_x <= 0;
            vga_y <= vga_y + 1;
            
            if (vga_y > 30)
                vga_y <= 0;
        end
    end

    char_display uut(
        .vga_clk(vga_clk),
        .vga_x(vga_x),
        .vga_y(vga_y),
        .ascii_value(65),
        .target_x(3),
        .target_y(4),
        .on(on)
    );
    
    initial begin
        
    
    end
endmodule
