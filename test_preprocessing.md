# Pre-Processing Functions Testing Guide

## Test Checklist for Section 3 - Pre-Processing

### 1. Pre-Processing Modal Access
- [ ] "Pre-Processing" button is visible in the main control panel
- [ ] "Pre-Processing" button opens the dedicated modal window
- [ ] Modal window has correct title "Pre-Processing Functions"
- [ ] Modal window has consistent size (700x450) and layout
- [ ] Preview area is visible on the left side (300x250)
- [ ] Control area with function categories is visible on the right side

### 2. CONTRAST Category Functions - Function Selection
- [ ] "CONTRAST:" category label is displayed
- [ ] "Adjust Contrast" button is functional
- [ ] "Histogram Eq" button is functional
- [ ] "Flatten BG" button is functional
- [ ] Each button switches to parameter control mode
- [ ] Selected function shows parameter interface

### 2.1 Adjust Contrast Parameters
- [ ] Function name "Adjust Contrast" is displayed at top
- [ ] "< Back" button returns to function selection
- [ ] Brightness slider is displayed (range: -100 to +100)
- [ ] Contrast slider is displayed (range: 0.1 to 3.0)
- [ ] Current values are displayed next to sliders
- [ ] Summary values are shown below sliders
- [ ] "Update Preview" button refreshes preview with current parameters
- [ ] Preview updates show brightness and contrast changes
- [ ] **Parameter Reset**: Brightness resets to 0.0 and contrast resets to 1.0 each time function is selected
- [ ] **Consistency**: Parameters always start with default values regardless of previous adjustments

### 2.2 Histogram Equalization Parameters
- [ ] Function name "Histogram Equalization" is displayed at top
- [ ] "< Back" button returns to function selection
- [ ] "Global" method button is functional
- [ ] "Adaptive (CLAHE)" method button is functional
- [ ] Selected method is clearly indicated with "<- Selected"
- [ ] CLAHE Clip Limit slider appears only in adaptive mode (range: 1.0 to 40.0)
- [ ] Current clip limit value is displayed
- [ ] "Update Preview" button appears in adaptive mode
- [ ] Preview updates automatically when method is changed
- [ ] **Parameter Reset**: Method resets to Global (0) and clip limit resets to 2.0 each time function is selected
- [ ] **Consistency**: Always starts with Global method regardless of previous selection

### 2.3 Flatten Background Parameters
- [ ] Function name "Flatten Background" is displayed at top
- [ ] "< Back" button returns to function selection
- [ ] Kernel size slider is displayed (range: 5 to 51)
- [ ] Slider automatically enforces odd numbers only
- [ ] Current kernel size is displayed next to slider
- [ ] Summary text shows current kernel size
- [ ] Helpful text explains "Larger values = more background removal"
- [ ] "Update Preview" button refreshes preview with current kernel size
- [ ] Preview shows background flattening effect
- [ ] **Parameter Reset**: Kernel size resets to 15 each time function is selected
- [ ] **Consistency**: Always starts with default kernel size regardless of previous adjustments

### 3. NOISE-REDUCTION Category Functions
- [ ] "NOISE-REDUCTION:" category label is displayed
- [ ] "Median Filter" button is functional
- [ ] "Wiener Filter" button is functional
- [ ] "Non-Local Means" button is functional
- [ ] Each button updates the preview when clicked
- [ ] Preview shows noise reduction effects

### 4. BLUR Category Functions
- [ ] "BLUR:" category label is displayed
- [ ] "Gaussian Blur" button is functional
- [ ] "Average Blur" button is functional
- [ ] "Sum Filter" button is functional
- [ ] "GS Dilate" button is functional
- [ ] "GS Erode" button is functional
- [ ] Each button updates the preview when clicked
- [ ] Preview shows appropriate blur/morphological effects

### 5. Preview System for Pre-Processing
- [ ] Preview area shows scaled version of processed image
- [ ] Preview updates immediately when function button is clicked
- [ ] Preview shows "No image loaded" when appropriate
- [ ] Preview correctly displays the effect of each function
- [ ] Preview maintains aspect ratio and proper scaling

### 6. Apply and Cancel Functionality
- [ ] "Apply" button applies the selected pre-processing function
- [ ] "Apply" button closes the modal window after applying
- [ ] "Cancel" button closes the modal window without applying changes
- [ ] Console output shows appropriate messages for each applied function
- [ ] Applied changes are visible in the main image display

### 7. Function-Specific Testing

#### 7.1 Adjust Contrast
- [ ] Function applies brightness and contrast adjustments using slider values
- [ ] Preview shows real-time contrast changes as sliders are moved
- [ ] Brightness range (-100 to +100) works correctly
- [ ] Contrast range (0.1 to 3.0) works correctly
- [ ] Console shows "Applied contrast adjustment: brightness=X, contrast=Y" message

#### 7.2 Histogram Equalization
- [ ] Global method applies standard histogram equalization
- [ ] Adaptive method applies CLAHE with specified clip limit
- [ ] Preview shows enhanced contrast for both methods
- [ ] Works correctly for both grayscale and color images
- [ ] CLAHE clip limit parameter (1.0 to 40.0) affects the result
- [ ] Console shows method type and clip limit in message

#### 7.3 Flatten Background
- [ ] Function applies morphological top-hat operation with specified kernel size
- [ ] Preview shows background flattening effect
- [ ] Kernel size parameter (5 to 51, odd only) affects the intensity
- [ ] Larger kernel sizes remove more background
- [ ] Console shows "Applied background flattening with kernel size=X" message

#### 7.4 Median Filter
- [ ] Function applies median filtering for noise reduction
- [ ] Preview shows noise reduction effect
- [ ] Console shows "Applied median filter" message

#### 7.5 Wiener Filter
- [ ] Function applies bilateral filtering (Wiener approximation)
- [ ] Preview shows noise reduction with edge preservation
- [ ] Console shows "Applied Wiener filter" message

#### 7.6 Non-Local Means
- [ ] Function applies non-local means denoising
- [ ] Preview shows advanced noise reduction
- [ ] Works correctly for both grayscale and color images
- [ ] Console shows "Applied non-local means denoising" message

#### 7.7 Gaussian Blur
- [ ] Function applies Gaussian blur
- [ ] Preview shows smooth blurring effect
- [ ] Console shows "Applied Gaussian blur" message

#### 7.8 Average Blur
- [ ] Function applies average blur
- [ ] Preview shows uniform blurring effect
- [ ] Console shows "Applied average blur" message

#### 7.9 Sum Filter
- [ ] Function applies sum filtering
- [ ] Preview shows filtering effect
- [ ] Console shows "Applied sum filter" message

#### 7.10 Grayscale Dilate
- [ ] Function applies morphological dilation
- [ ] Preview shows dilation effect (bright areas expand)
- [ ] Console shows "Applied grayscale dilation" message

#### 7.11 Grayscale Erode
- [ ] Function applies morphological erosion
- [ ] Preview shows erosion effect (bright areas shrink)
- [ ] Console shows "Applied grayscale erosion" message

### 8. Error Handling
- [ ] Modal shows appropriate message when no image is loaded
- [ ] Functions handle grayscale and color images appropriately
- [ ] Error messages are displayed in console for invalid operations
- [ ] Application doesn't crash with invalid inputs
- [ ] Modal can be safely closed and reopened multiple times

### 9. Integration with Main System
- [ ] Pre-processing functions work with images loaded through "Load Image"
- [ ] Pre-processing can be applied after other image processing operations
- [ ] "Reset Image" correctly restores original image after pre-processing
- [ ] Multiple pre-processing operations can be applied sequentially
- [ ] Pre-processing works correctly with different image formats and sizes

### 10. Parameter Reset Functionality
- [ ] **Adjust Contrast Reset Test**:
  - [ ] Select Adjust Contrast, change brightness to 50 and contrast to 2.0
  - [ ] Click "< Back" to return to function selection
  - [ ] Select Adjust Contrast again
  - [ ] Verify brightness shows 0.0 and contrast shows 1.0 (default values)
- [ ] **Histogram Equalization Reset Test**:
  - [ ] Select Histogram Eq, change to Adaptive method and set clip limit to 10.0
  - [ ] Click "< Back" to return to function selection
  - [ ] Select Histogram Eq again
  - [ ] Verify method shows Global and clip limit shows 2.0 (default values)
- [ ] **Flatten Background Reset Test**:
  - [ ] Select Flatten BG, change kernel size to 31
  - [ ] Click "< Back" to return to function selection
  - [ ] Select Flatten BG again
  - [ ] Verify kernel size shows 15 (default value)
- [ ] **Cross-Function Reset Test**:
  - [ ] Adjust parameters in one function, switch to another function, then return
  - [ ] Verify parameters are reset each time a function is selected
- [ ] **Apply/Cancel Reset Test**:
  - [ ] Adjust parameters, click Apply or Cancel to close modal
  - [ ] Reopen Pre-Processing modal and select the same function
  - [ ] Verify parameters are reset to defaults

### 11. User Experience
- [ ] Function buttons are clearly labeled and intuitive
- [ ] Category organization makes functions easy to find
- [ ] Preview provides immediate visual feedback
- [ ] Modal window layout is clean and organized
- [ ] Console messages provide clear feedback on operations
- [ ] Function selection is clearly indicated at the bottom of the modal
- [ ] Parameter reset provides consistent starting point for each function

## Test Results
Date: ___________
Tester: ___________
Status: ___________
Notes: ___________

## Performance Notes
- [ ] Pre-processing functions execute in reasonable time
- [ ] Preview updates are responsive
- [ ] No memory leaks during extended use of pre-processing functions
- [ ] Modal window opens and closes smoothly
