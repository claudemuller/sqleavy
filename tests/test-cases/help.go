package tests

func TestHelp() error {
	expected := []string{
		"db > .help - this menu",
		".exit - exit this program",
		"db >",
	}
	cmds := []string{
		".help\n",
		".exit\n",
	}

	return runTest(cmds, expected)
}
