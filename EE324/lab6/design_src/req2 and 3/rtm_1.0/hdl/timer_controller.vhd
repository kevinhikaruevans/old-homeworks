----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/24/2020 04:07:12 PM
-- Design Name: 
-- Module Name: timer_controller - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity timer_controller is
    Port ( clk           : in STD_LOGIC;
           btn_startstop : in STD_LOGIC;
           btn_inc       : in STD_LOGIC;
           btn_reset     : in STD_LOGIC;
           count_out     : out STD_LOGIC_VECTOR (13 downto 0));
end timer_controller;

architecture Behavioral of timer_controller is
    type timer_state is (IDLE, RESET, RESETTING, INCREMENT, INCREMENTING, STOPPING, RUN, RUNNING);
    signal state : timer_state := IDLE;
    signal count : std_logic_vector(13 downto 0) := (others => '0');
begin
    count_out <= count;
    
    -- just gonna do this using the clk instead of combo logic on the btns :/
    
    state_machine : process(clk) begin -- btn_startstop, btn_inc, btn_reset
        if rising_edge(clk) then
        case state is
            when IDLE => 
                if (btn_startstop = '1') then
                    state <= RUN;
                elsif (btn_reset = '1') then
                    state <= RESET;
                elsif (btn_inc = '1') then
                    state <= INCREMENT;
                end if;
                
            when RESET =>
                if (btn_reset = '0') then
                    state <= RESETTING;
                else
                    state <= RESET;
                end if;
                
            when RESETTING =>
                state <= IDLE;
            
            when INCREMENT =>
                if (btn_inc = '0') then
                    state <= INCREMENTING;
                 else
                    state <= INCREMENT;
                end if;
                
            when INCREMENTING =>
                state <= IDLE;
            
            when RUN =>
                if (btn_startstop = '0') then
                    state <= RUNNING;
                else
                    state <= RUN;
                end if;
                
            when RUNNING =>
                if (btn_startstop = '1') then
                    state <= STOPPING;
                else
                    state <= RUNNING;
                end if;
                
            when STOPPING =>
                if (btn_startstop = '0') then
                    state <= IDLE;
                else
                    state <= STOPPING;
                end if;

            when others =>
                state <= RUNNING;
        end case;
        
        end if;
    end process;

    run_counter : process(clk) begin
        if rising_edge(clk) then
            if state = RUNNING then
                if unsigned(count) = 9999 then
                    count <= (others => '0');
                else
                    count <= std_logic_vector( unsigned(count) + 1);
                end if;
            elsif state = RESETTING then
                count <= (others => '0');
            elsif state = INCREMENTING then
                count <= std_logic_vector(unsigned(count) + 1);
            end if;
        end if;
    end process;
end Behavioral;
