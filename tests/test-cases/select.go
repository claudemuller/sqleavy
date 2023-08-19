package tests

func TestSelect() error {
	expected := []string{
		"db > Statement executed.",
		"db > db > (1, user1, user1@example.com)",
		"Statement executed.",
		"db > ",
	}
	cmds := []string{
		"insert 1 user1 user1@example.com\n",
		"select",
		".exit\n",
	}

	return runTest(cmds, expected)
}
