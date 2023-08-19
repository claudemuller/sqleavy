package tests

func TestInsert() error {
	expected := []string{
		"db > Statement executed.",
		"db >",
	}
	cmds := []string{
		"insert 1 user1 user1@example.com\n",
		".exit\n",
	}

	return runTest(cmds, expected)
}

func TestInsert1400() error {
	expected, cmds := buildExpectedAndCmds(1400)
	cmds = append(cmds, ".exit\n")
	expected = append(expected, "db >")

	return runTest(cmds, expected)
}

func TestInsert1401() error {
	expected, cmds := buildExpectedAndCmds(1401)
	cmds = append(cmds, ".exit\n")
	expected[1400] = "db > Error, table is full."
	expected = append(expected, "db >")

	return runTest(cmds, expected)
}

func buildExpectedAndCmds(n int) ([]string, []string) {
	expected := make([]string, 0, n+1)
	cmds := make([]string, 0, n+1)

	for i := 0; i < n; i++ {
		cmds = append(cmds, "insert 1 user1 user1@example.com\n")
		expected = append(expected, "db > Statement executed.")
	}

	return expected, cmds
}
