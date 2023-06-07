`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/28/2020 02:19:49 PM
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
        input vgaclk,
        input[3:0] btn,
        output HS, VS, vde,
        output [3:0] vga_r, vga_g, vga_b
    );
    
    wire video_active;
    wire [9:0] x, y;
    
    vga_syncgen gen(
        .clk(vgaclk),
        .rst(1'b0),
        .hsync(HS),
        .vsync(VS),
        .video_active(video_active),
        
        .x(x),
        .y(y)
    );
    
    assign vde = video_active;

    
    /*wire on = video_active && (x <= (320 + 16) && x > (320 - 16))
        && (y <= (240 + 16) && y > (240 - 16));*/
    wire on;
            
            
    // the x & y coords of the object to draw
    // ...initially for the crosshairs
    wire [9:0] cross_x, cross_y;
        
    
    
    // For requirement 4, draw a white screen:
    //assign on = 1;
    
    // Requirement 5: crosshairs
    // assign on = video_active && (x == 320 || y == 240);
    
    // Requirement 6: box with params
    //disp_box #( .SIZE(32) )
    //    box (
    //        .x(x),
    //        .y(y),
    //        .video_active(video_active),
    //        .on(on)
    //    ); 
    
    // Challenge 1: moving the crosshairs
    //    movement_controller(
    //        .clk(vgaclk),
    //        .rst(0),
    //        .x(cross_x),
    //        .y(cross_y)
    //    );
    //    assign on = video_active && (x == cross_x || y == cross_y);

    // Challenge 2: moving the box
    //    movement_controller(
    //        .clk(vgaclk),
    //        .rst(0),
    //        .x(cross_x),
    //        .y(cross_y)
    //    );
    //    disp_box #( .SIZE(32) )
    //        box (
    //            .box_x(cross_x),
    //            .box_y(cross_y),
    //            .x(x),
    //            .y(y),
    //            .video_active(video_active),
    //            .on(on)
    //        );         
    
    // Challenge 3: moving the box/crosshair
    btn_controller(
        .clk(vgaclk),
        .rst(0),
        .btn(btn),
        .x(cross_x),
        .y(cross_y)
    );
    disp_box #( .SIZE(32) )
        box (
            .box_x(cross_x),
            .box_y(cross_y),
            .x(x),
            .y(y),
            .video_active(video_active),
            .on(on)
        );   

    //assign on = video_active && (x == cross_x || y == cross_y);

    assign vga_r = on ? 4'b1111 : 0;
    assign vga_g = on ? 4'b1111 : 0;
    assign vga_b = on ? 4'b1111 : 0;
endmodule
