library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library reconos_v3_01_a;
use reconos_v3_01_a.reconos_pkg.all;

library rt_framegrabber_v1_00_a;
use rt_framegrabber_v1_00_a.reconos_thread_pkg.all;

entity rt_framegrabber is
	port (
		-- OSIF FIFO ports
		OSIF_Sw2Hw_Data    	: in  std_logic_vector(31 downto 0);
		OSIF_Sw2Hw_Empty   	: in  std_logic;
		OSIF_Sw2Hw_RE      	: out std_logic;

		OSIF_Hw2Sw_Data    	: out std_logic_vector(31 downto 0);
		OSIF_Hw2Sw_Full    	: in  std_logic;
		OSIF_Hw2Sw_WE      	: out std_logic;

		-- MEMIF FIFO ports
		MEMIF_Hwt2Mem_Data  : out std_logic_vector(31 downto 0);
		MEMIF_Hwt2Mem_Full  : in  std_logic;
		MEMIF_Hwt2Mem_WE    : out std_logic;

		MEMIF_Mem2Hwt_Data  : in  std_logic_vector(31 downto 0);
		MEMIF_Mem2Hwt_Empty : in  std_logic;
		MEMIF_Mem2Hwt_RE    : out std_logic;

		-- HDMI input
		HDMI_IN_DIN 		: in  std_logic_vector(63 downto 0);
		HDMI_IN_WR_EN		: in  std_logic;
		HDMI_IN_WR_OVERFL	: out std_logic;
		HDMI_IN_WR_SYNC		: in  std_logic;
		HDMI_IN_WR_CK		: in  std_logic;

		HWT_Clk    			: in  std_logic;
		HWT_Rst    			: in  std_logic;
		HWT_Signal			: in  std_logic;
		HWT_MBREAD 			: out std_logic
	);
end entity rt_framegrabber;

architecture implementation of rt_framegrabber is
	signal i_osif  : i_osif_t;
	signal o_osif  : o_osif_t;
	signal i_memif : i_memif_t;
	signal o_memif : o_memif_t;


	--RAM 
	constant C_LINE_LEN_MATRIX : integer := 480*640*4;
	-- const for matrixes A and C
	constant C_LOCAL_RAM_SIZE_MATRIX_A_C          : integer := C_LINE_LEN_MATRIX;
	constant C_LOCAL_RAM_ADDR_WIDTH_MATRIX_A_C    : integer := 21;
	constant C_LOCAL_RAM_SIZE_IN_BYTES_MATRIX_A_C : integer := 4 * C_LOCAL_RAM_SIZE_MATRIX_A_C;
	type LOCAL_MEMORY_TYPE_MATRIX_A_C is array(0 to C_LOCAL_RAM_SIZE_MATRIX_A_C - 1) of std_logic_vector(31 downto 0);
    shared variable local_ram_a : LOCAL_MEMORY_TYPE_MATRIX_A_C;
        

	signal i_ram_A : i_ram_t;
	signal o_ram_A : o_ram_t;


	
	signal o_RAM_A_Addr_reconos   : std_logic_vector(0 to C_LOCAL_RAM_ADDR_WIDTH_MATRIX_A_C - 1); 
	signal o_RAM_A_Addr_reconos_2 : std_logic_vector(0 to 31);
	signal o_RAM_A_Data_reconos   : std_logic_vector(0 to 31);
	signal o_RAM_A_WE_reconos     : std_logic;
	signal i_RAM_A_Data_reconos   : std_logic_vector(0 to 31);

	signal o_RAM_A_Addr_mul : std_logic_vector(0 to C_LOCAL_RAM_ADDR_WIDTH_MATRIX_A_C - 1);
	signal i_RAM_A_Data_mul : std_logic_vector(0 to 31);

    signal len_data_MATRIX_A_C : std_logic_vector(31 downto 0);
	--RAM END

	signal rb_info		: unsigned(31 downto 0);
    signal ignore       : std_logic_vector(31 downto 0);
    signal ret          : std_logic_vector(31 downto 0);
	
	type STATE_TYPE is (STATE_THREAD_INIT,STATE_INIT_DATA,STATE_RUN, STATE_COPY, STATE_WAIT, STATE_EXIT );
	signal state : STATE_TYPE;

begin
	osif_setup (
		i_osif,
		o_osif,
		OSIF_Sw2Hw_Data,
		OSIF_Sw2Hw_Empty,
		OSIF_Sw2Hw_RE,
		OSIF_Hw2Sw_Data,
		OSIF_Hw2Sw_Full,
		OSIF_Hw2Sw_WE
	);

	memif_setup (
		i_memif,
		o_memif,
		MEMIF_Mem2Hwt_Data,
		MEMIF_Mem2Hwt_Empty,
		MEMIF_Mem2Hwt_RE,
		MEMIF_Hwt2Mem_Data,
		MEMIF_Hwt2Mem_Full,
		MEMIF_Hwt2Mem_WE
	);

	osfsm_proc: process (HWT_Clk,HWT_Rst,o_osif,o_memif) is
		variable resume, done : boolean;
		
		variable counter : unsigned(31 downto 0);
	begin
		if HWT_Rst = '1' then
			osif_reset(o_osif);
			memif_reset(o_memif);
			state <= STATE_THREAD_INIT;
			counter := (others => '0');
            len_data_MATRIX_A_C <= std_logic_vector(to_unsigned(C_LOCAL_RAM_SIZE_IN_BYTES_MATRIX_A_C, 32));
            
		elsif rising_edge(HWT_Clk) then
			case state is
				when STATE_THREAD_INIT =>
					osif_read(i_osif, o_osif, ignore, done);
					if done then
						state <= STATE_INIT_DATA;
					end if;
					
				when STATE_INIT_DATA =>
                     osif_get_init_data(i_osif, o_osif, ret, done);
                     if done then
					 	if ret /= x"00000000" then
							rb_info <= unsigned(ret);
							state <= STATE_RUN;
						else
							state <= STATE_INIT_DATA;
						end if;
                     end if;
			
				when STATE_RUN => 
					state <= STATE_COPY;

				when STATE_COPY => 
					memif_write(i_ram_A, o_ram_A, i_memif, o_memif, X"00000000", std_logic_vector(rb_info), len_data_MATRIX_A_C, done);
					if done then
						state <= STATE_WAIT;
					end if;
				when STATE_WAIT => 
					if counter < x"01000000" then
						counter := counter + x"00000001";
					else
						counter := (others => '0');
						state <= STATE_COPY;
					end if;
					 
            	when STATE_EXIT => 
					osif_thread_exit (i_osif, o_osif);
			end case;
		end if;
	end process osfsm_proc;


	hdmi_proc: process (HDMI_IN_WR_CK,HWT_Rst) is
		variable counter : unsigned(31 downto 0);
	begin
		if HWT_Rst = '1' then
			counter := (others => '0');
			o_RAM_A_WE_reconos <= '0';	
		elsif rising_edge(HDMI_IN_WR_CK) then
			if HDMI_IN_WR_SYNC = '1' then
				counter := (others => '0');
			else
				counter := counter + 1;
			end if;

			o_RAM_A_Data_reconos <= HDMI_IN_DIN(31 downto 0);
			o_RAM_A_Addr_reconos <= std_logic_vector(counter(C_LOCAL_RAM_ADDR_WIDTH_MATRIX_A_C-1 downto 0));
			o_RAM_A_WE_reconos 	 <= '1';		
		end if;
	end process hdmi_proc;


-- local BRAM read and write access
	local_ram_ctrl_1 : process (HDMI_IN_WR_CK) is
	begin
		if (rising_edge(HDMI_IN_WR_CK)) then
			if (o_RAM_A_WE_reconos = '1') then
				local_ram_A(to_integer(unsigned(o_RAM_A_Addr_reconos))) := o_RAM_A_Data_reconos;
			end if;
		end if;
	end process local_ram_ctrl_1;
	

end architecture;

