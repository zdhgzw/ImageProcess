# Unified Modal Window System Testing Guide

## Test Checklist for Image Processing Application

### 1. Application Startup
- [ ] Application starts without errors
- [ ] Main window opens with correct size (1000x650)
- [ ] Image display area is visible at the top
- [ ] Control panel with 7 function buttons is visible below image area
- [ ] No modal windows are open initially

### 2. Unified Modal Window System
- [ ] Each function button opens exactly one dedicated modal window
- [ ] Modal windows have consistent layout and styling
- [ ] All modal windows have proper titles indicating the function name
- [ ] Modal windows are properly sized and positioned

### 3. Load Image Modal
- [ ] "Load Image" button opens Load Image modal
- [ ] Modal shows "Browse Files" button
- [ ] File dialog opens when "Browse Files" is clicked
- [ ] Can navigate to test image (build/bin/images/C-2.jpg)
- [ ] Image loads successfully and modal closes automatically
- [ ] Current filename is displayed in control panel
- [ ] Image size is displayed in control panel
- [ ] "Cancel" button closes modal without loading

### 4. Reset Image Modal
- [ ] "Reset Image" button opens Reset Image modal
- [ ] Modal shows confirmation message and explanation
- [ ] "Reset" button resets image and closes modal
- [ ] "Cancel" button closes modal without resetting
- [ ] Console shows "Reset to original image" message
- [ ] Modal shows appropriate message when no image is loaded

### 5. Grayscale Modal
- [ ] "Grayscale" button opens Grayscale modal
- [ ] Modal shows preview area on the left
- [ ] Preview shows grayscale version of the image
- [ ] Modal shows explanation text on the right
- [ ] "Apply" button converts image and closes modal
- [ ] "Cancel" button closes modal without applying
- [ ] Console shows "Converted to grayscale" message

### 6. Color Select Modal
- [ ] "Color Select" button opens Color Select modal
- [ ] Modal shows preview area on the left
- [ ] Modal shows HSV sliders on the right
- [ ] Sliders control Hue, Saturation, and Value ranges
- [ ] Current HSV values are displayed
- [ ] "Update Preview" button refreshes preview
- [ ] Preview shows color selection effect
- [ ] "Apply" button applies effect and closes modal
- [ ] "Cancel" button closes modal without applying

### 7. Color Clustering Modal
- [ ] "Clustering" button opens Color Clustering modal
- [ ] Modal shows preview area on the left
- [ ] Modal shows K-clusters slider on the right (range 2-8)
- [ ] Current K value is displayed
- [ ] "Update Preview" button refreshes preview
- [ ] Preview shows clustering effect
- [ ] "Apply" button applies effect and closes modal
- [ ] "Cancel" button closes modal without applying

### 8. Color Deconvolution Modal
- [ ] "Deconvolution" button opens Color Deconvolution modal
- [ ] Modal shows preview area on the left
- [ ] Modal shows channel selection buttons (Blue/Green/Red)
- [ ] Selected channel is clearly indicated
- [ ] Clicking channel buttons updates selection and preview
- [ ] Preview shows selected channel extraction
- [ ] "Apply" button applies effect and closes modal
- [ ] "Cancel" button closes modal without applying

### 9. Channel Operations Modal
- [ ] "Channel Ops" button opens Channel Operations modal
- [ ] Modal shows preview area on the left
- [ ] Modal shows operation type buttons (Add/Subtract/Multiply/Divide)
- [ ] Modal shows value slider
- [ ] Selected operation and current value are displayed
- [ ] "Update Preview" button refreshes preview
- [ ] Preview shows operation effect
- [ ] "Apply" button applies effect and closes modal
- [ ] "Cancel" button closes modal without applying

### 10. Modal Window Consistency
- [ ] All modal windows have the same size and positioning
- [ ] All modal windows have consistent button placement
- [ ] All modal windows have consistent preview area layout
- [ ] All modal windows have consistent styling and fonts
- [ ] All modal windows properly handle "Apply" and "Cancel" actions

### 11. Preview System
- [ ] Preview area shows scaled version of processed image
- [ ] Preview updates correctly when parameters change
- [ ] Preview shows "No image loaded" when appropriate
- [ ] Preview shows "No preview" when preview is not available

### 12. Error Handling and Edge Cases
- [ ] Functions show appropriate message when no image is loaded
- [ ] Canceling file dialog doesn't crash the application
- [ ] Invalid operations show error messages in console
- [ ] Application handles missing images gracefully
- [ ] Multiple rapid button clicks don't cause issues
- [ ] ESC key closes the application properly

### 13. User Experience
- [ ] Consistent interaction pattern across all functions
- [ ] Clear visual feedback for all user actions
- [ ] Intuitive button labels and modal titles
- [ ] Appropriate spacing and layout in all modal windows
- [ ] Console output provides clear feedback for all operations

## Test Results
Date: ___________
Tester: ___________
Status: ___________
Notes: ___________

## Performance Notes
- [ ] Modal windows open quickly without lag
- [ ] Preview updates are responsive
- [ ] Image processing operations complete in reasonable time
- [ ] No memory leaks or performance degradation during extended use
