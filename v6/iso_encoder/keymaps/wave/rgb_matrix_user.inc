RGB_MATRIX_EFFECT(hwelle)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
// The "hwelle_hit_tracker_t" type and external "hwelle_tracker" are imported from keymap.c
typedef struct {
    uint8_t x;
    uint8_t y;
    uint32_t tick;
} hwelle_hit_tracker_t;
extern hwelle_hit_tracker_t hwelle_tracker;


// Animation
static bool hwelle(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min,led_max);
    rgb_matrix_set_flags(LED_FLAG_ALL);
    // Set the static background color
    hsv_t base_hsv = rgb_matrix_config.hsv;
    base_hsv.v = 80; // approximately 30% brightness
    rgb_t base_color = rgb_matrix_hsv_to_rgb(base_hsv);
    rgb_matrix_set_color_all(base_color.r, base_color.g, base_color.b);


    // Check if the effect should still be active
    uint32_t elapsed_time = timer_read32() - hwelle_tracker.tick;
    if (elapsed_time > 1000) { // Stop the splash effect after 1 second
        return rgb_matrix_check_finished_leds(led_max);
    }


    // Apply the brighter splash effect over the background


    // Scale the elapsed time to control the "radius" speed of the splash
    uint16_t tick = scale16by8(elapsed_time, qadd8(rgb_matrix_config.speed, 1));

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        // Calculate the distance of each LED to the keypress point
        int16_t dx = g_led_config.point[i].x - hwelle_tracker.x;
        int16_t dy = g_led_config.point[i].y - hwelle_tracker.y;
        uint16_t dist = sqrt16(dx * dx + dy * dy);

        // 'tick' is the current wave radius.
        // 'ripple_width' is the width of the color gradient.
        uint8_t ripple_width = 48;
        // 'delta' is the LED's distance from the wave front.
        int16_t delta = dist - tick;

        // Is the LED within the visible wave?
        if (delta < 0 && delta > -ripple_width) {
            // Yes. Calculate brightness based on proximity to the wave front.
            // creates a smooth transition from 255 (bright) to 0 (dark).
            uint8_t splash_brightness = 255 - ((uint16_t)abs(delta) * 255) / ripple_width;

            // Draw the splash color only if it's brighter than the background.
            if (splash_brightness > base_hsv.v) {
                hsv_t splash_hsv = rgb_matrix_config.hsv; // Use global color values
                splash_hsv.v = splash_brightness;
                rgb_t splash_color = rgb_matrix_hsv_to_rgb(splash_hsv);
                rgb_matrix_set_color(i, splash_color.r, splash_color.g, splash_color.b);
            }
        }
        // If the LED is not in the wave, nothing happens, and the
        // previously set background color remains visible.
    }

    return rgb_matrix_check_finished_leds(led_max);
}


#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
