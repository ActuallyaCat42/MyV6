RGB_MATRIX_EFFECT(hwelle)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS



static hsv_t hwelle_math(hsv_t hsv, uint8_t dist, uint16_t tick) {
    uint16_t effect = tick - dist;
    if (effect > 255) {
        effect = 255;
    }
    hsv.v = qadd8(hsv.v, 255 - effect);
    return hsv;
}

static bool hwelle_runner(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Base brightness
    hsv_t base_hsv = rgb_matrix_config.hsv;
    base_hsv.v = 80;
    rgb_t base_color = rgb_matrix_hsv_to_rgb(base_hsv);
    rgb_matrix_set_color_all(base_color.r, base_color.g, base_color.b);

    // was there a press?
    if (g_last_hit_tracker.count == 0) {
        return rgb_matrix_check_finished_leds(led_max);
    }
    // last keypress ist last in the array
    uint8_t last_hit_index = g_last_hit_tracker.count - 1;

    // time since last keypress
    uint16_t elapsed_time = g_last_hit_tracker.tick[last_hit_index];
    uint16_t tick = scale16by8(elapsed_time, qadd8(rgb_matrix_config.speed, 1));



    // the running part
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        hsv_t hsv = base_hsv;
        // now with the actual build in tracker
        int16_t  dx   = g_led_config.point[i].x - g_last_hit_tracker.x[last_hit_index];
        int16_t  dy   = g_led_config.point[i].y - g_last_hit_tracker.y[last_hit_index];
        uint8_t  dist = sqrt16(dx * dx + dy * dy);

        hsv = hwelle_math(hsv, dist, tick);

        if (hsv.v > base_hsv.v) {
            rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }

    return rgb_matrix_check_finished_leds(led_max);
}

// Getting everyting togehter
static bool hwelle(effect_params_t* params) {
    return hwelle_runner(params);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
