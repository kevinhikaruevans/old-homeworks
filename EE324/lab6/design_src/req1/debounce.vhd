----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/22/2020 09:06:44 AM
-- Design Name: 
-- Module Name: debounce - Behavioral
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
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity debounce is
    Port ( signal_in : in STD_LOGIC;
           signal_out : out STD_LOGIC := '0';
           clk : in STD_LOGIC);
end debounce;

architecture Behavioral of debounce is
    type state_type is (WAITING, SETTLING, RESETTING, RESETWAIT);
    signal state : state_type := WAITING;
begin
    process (clk)
    begin
        if rising_edge(clk) then
            case state is 
                when WAITING =>
                    signal_out <= '0';
                    
                    if (signal_in = '1') then
                        state <= SETTLING;
                    end if;
                when SETTLING =>
                    signal_out <= '0';
                    
                    if (signal_in = '1') then
                        state <= RESETTING;
                    end if;
                when RESETTING =>
                    signal_out <= '1';
                    
                    if (signal_in = '0') then
                        state <= RESETWAIT;
                    end if;
                
                when RESETWAIT =>
                    state <= WAITING;
                
                when others =>
                    state <= WAITING;
                    signal_out <= '0';
            end case;
        end if;
    end process;

end Behavioral;
