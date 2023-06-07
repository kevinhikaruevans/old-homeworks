`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/01/2020 05:40:17 PM
// Design Name: 
// Module Name: disp_box
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


module disp_box
    #(
        parameter BOX_X = 320,
        parameter BOX_Y = 240,
        parameter SIZE = 32
    )
    (
        input[9:0] x, y,         // vga x, y
        input[9:0] box_x, box_y, // box coords
        input video_active,
        output on
    );
    
// for requirement 6:
//    assign on = video_active && (x <= (BOX_X + SIZE/2) && x > (BOX_X - SIZE/2))
//        && (y <= (BOX_Y + SIZE/2) && y > (BOX_Y - SIZE/2));

// for challenge 2:
    assign on = video_active && (x <= (box_x + SIZE/2) && x > (box_x - SIZE/2))
        && (y <= (box_y + SIZE/2) && y > (box_y - SIZE/2)); 
endmodule
