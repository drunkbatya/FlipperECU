# What is this?

Some neccecery functions are disabled in the Flipper API
```bash
scons: *** [/Users/drunkbatya/.ufbt/build/flipper_ecu.impsyms] /Users/drunkbatya/.ufbt/build/flipper_ecu.fap: app may not be runnable. Symbols not resolved using firmware's API: {'LL_ADC_Init', 'LL_ADC_CommonInit', 'LL_ADC_REG_Init'} (in API, but disabled: {'LL_ADC_Init', 'LL_ADC_CommonInit', 'LL_ADC_REG_Init'})
```

So, i'm just copying neccecerry functions from `stm32wbxx_ll_adc.c` file here.
