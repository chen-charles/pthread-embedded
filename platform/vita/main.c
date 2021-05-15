extern void pte_test_main();

static void __attribute__((constructor)) entry_point()
{
  pte_test_main();
}
