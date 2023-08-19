package main

func main() {
	if err := tests.TestHelp(); err != nil {
		tests.ReportFailure(err)
		return
	}
	tests.ReportSuccess("Help")

	if err := tests.TestInsert(); err != nil {
		tests.ReportFailure(err)
		return
	}
	tests.ReportSuccess("Insert")

	if err := tests.TestSelect(); err != nil {
		tests.ReportFailure(err)
		return
	}
	tests.ReportSuccess("Select")
}
