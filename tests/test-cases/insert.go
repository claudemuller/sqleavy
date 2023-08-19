package tests

import (
	"fmt"
	"strings"
)

func TestInsert() error {
	expected, cmds := buildExpectedAndCmds(1)
	cmds = append(cmds, ".exit\n")
	expected = append(expected, "db >")

	return runTest(cmds, expected)
}

func TestInsertMaxStrings() error {
	longUser := buildStr("a", 32)
	longEmail := buildStr("a", 255)

	expected := []string{
		"db > Statement executed.",
		fmt.Sprintf("db > (1, %s, %s)", longUser, longEmail),
		"Statement executed.",
		"db >",
	}

	cmds := []string{
		fmt.Sprintf("insert 1 %s %s\n", longUser, longEmail),
		"select\n",
		".exit\n",
	}

	return runTest(cmds, expected)
}

func TestInsert1300() error {
	expected, cmds := buildExpectedAndCmds(1300)
	cmds = append(cmds, ".exit\n")
	expected = append(expected, "db >")

	return runTest(cmds, expected)
}

func TestInsert1301() error {
	expected, cmds := buildExpectedAndCmds(1301)
	cmds = append(cmds, ".exit\n")
	expected[1300] = "db > Error, table is full."
	expected = append(expected, "db >")

	return runTest(cmds, expected)
}

func buildStr(c string, n int) string {
	str := make([]string, 0, n)
	for i := 0; i < n; i++ {
		str = append(str, c)
	}
	return strings.Join(str, "")
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
