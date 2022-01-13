SECTIONS
{
	. = 0xC0001000;

	.text 0xC0001000 : AT (ADDR (.text) - 0xC0000000) {
		*(.text)
	}
}