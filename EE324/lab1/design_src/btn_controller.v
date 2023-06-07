`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/01/2020 07:01:50 PM
// Design Name: 
// Module Name: btn_controller
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


module btn_controller
    #(
        parameter
        DEFAULT_X = 320,
        DEFAULT_Y = 240,
        CLOCKDIV = 15
    )
    (
        input clk,
        input rst,
        
        input[3:0] btn,
        output [9:0] x, y
    );
    
    reg [CLOCKDIV:0] counter = 0;
    
    //(* mark_debug = "true", keep = "true" *)
    reg [9:0] _x = DEFAULT_X;
    
    //(* mark_debug = "true", keep = "true" *)
    reg [9:0] _y = DEFAULT_Y;
    
    always @(posedge(clk)) begin
        if (rst) begin
            _x      <= DEFAULT_X;
            _y      <= DEFAULT_Y;
            counter <= 0;
        end
        else if (counter == 16'b0) begin
            counter <= 0;
            
            /* vi keys, I guess */
            case (btn)
                4'b1000: _x <= _x - 1;
                4'b0100: _y <= _y + 1;
                4'b0010: _y <= _y - 1;
                4'b0001: _x <= _x + 1;
            endcase
        end
        
        counter <= counter + 1;
    end

    assign x = _x;
    assign y = _y;
endmodule
