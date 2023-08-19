package tests

func TestInsert() error {
	expected := []string{
		"db > Statement executed.",
		"db > db >",
	}
	cmds := []string{
		"insert 1 user1 user1@example.com\n",
		".exit\n",
	}

	return runTest(cmds, expected)
}
