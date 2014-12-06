// Cloud API
// =========

// this function automagically gets called upon a matching POST request
int handleParams(String command)
{
  //look for the matching argument "coffee" <-- max of 64 characters long
  int p = 0;
  while (p<(int)command.length()) {
    int i = command.indexOf(',',p);
    if (i<0) i = command.length();
    int j = command.indexOf('=',p);
    if (j<0) break;
    String key = command.substring(p,j);
    String value = command.substring(j+1,i);
    int val = value.toInt();
    // global params
    if (key=="wait")
      cycle_wait = val;
    else if (key=="mode")
      mode = val;
    else if (key=="brightness")
      brightness = val;
    else if (key=="fade_base")
      fade_base = val;
    // simple lamp params
    else if (key=="lamp_red")
      lamp_red = val;
    else if (key=="lamp_green")
      lamp_green = val;
    else if (key=="lamp_blue")
      lamp_blue = val;
    // text color params
    else if (key=="red_text")
      red_text = val;
    else if (key=="green_text")
      green_text = val;
    else if (key=="blue_text")
      blue_text = val;
    // text params
    else if (key=="cycles_per_px")
      cycles_per_px = val;
    else if (key=="text_repeats")
      text_repeats = val;
    else if (key=="text_base_line")
      text_base_line = val;
    else if (key=="fade_per_repeat")
      fade_per_repeat = val;
    else if (key=="text_intensity")
      text_intensity = val;
    // torch color params
    else if (key=="red_bg")
      red_bg = val;
    else if (key=="green_bg")
      green_bg = val;
    else if (key=="blue_bg")
      blue_bg = val;
    else if (key=="red_bias")
      red_bias = val;
    else if (key=="green_bias")
      green_bias = val;
    else if (key=="blue_bias")
      blue_bias = val;
    else if (key=="red_energy")
      red_energy = val;
    else if (key=="green_energy")
      green_energy = val;
    else if (key=="blue_energy")
      blue_energy = val;
    // torch params
    else if (key=="spark_prob") {
      random_spark_probability = val;
      resetEnergy();
    }
    else if (key=="spark_cap")
      spark_cap = val;
    else if (key=="spark_tfr")
      spark_tfr = val;
    else if (key=="side_rad")
      side_rad = val;
    else if (key=="up_rad")
      up_rad = val;
    else if (key=="heat_cap")
      heat_cap = val;
    else if (key=="flame_min")
      flame_min = val;
    else if (key=="flame_max")
      flame_max = val;
    else if (key=="spark_min")
      spark_min = val;
    else if (key=="spark_max")
      spark_max = val;
    else if (key=="upside_down")
      upside_down = val;
    p = i+1;
  }
  return 1;
}
