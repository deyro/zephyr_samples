## Commands to build and run ztest on hardware

zephyr version - v2.5.0

```
cd $HOME/zephyrproject/zephyr/

scripts/twister --board-root <full_path_to_boards_directory> --device-testing --device-serial /dev/ttyACM0 -p stm32g474re_disco -T <full_path_to_project_source_directory>/tests/<test_name> -O <path_to_output_directory>

scripts/twister --board-root $HOME/zephyr-workspace2/zephyr-snippets/ztest_trial/source/boards/ --device-testing --device-serial /dev/ttyACM0 -p stm32g474re_disco -T $HOME/zephyr-workspace2/zephyr-snippets/ztest_trial/source/tests/integration -O $HOME/zephyr-workspace2/zephyr-snippets/ztest_trial/source/tests/testoutdir
```