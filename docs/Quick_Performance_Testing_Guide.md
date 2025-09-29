# Quick Performance Testing Guide

## 🚀 **Immediate Testing Steps**

While the build completes, here's how to quickly test the new performance profiling system:

### **Step 1: Build Completion Check**
The build is currently running. Once complete, you should see:
```
Build succeeded.
```

### **Step 2: Launch Editor**
```bash
# Double-click DarkAge.uproject
# Or use the task: Launch Editor
```

### **Step 3: Basic Performance Test**
```bash
# Open console with ` key and try these commands:

# 1. Check if profiler is loaded
stat unit

# 2. Basic performance stats
ShowPerformanceStats

# 3. Quick test with few NPCs
RunPerformanceTest 50

# 4. If that works, try larger test
RunPerformanceTest 100
```

### **Step 4: Expected Console Output**
You should see something like:
```
=== Current Performance Stats ===
Frame Time: 18.45 ms (54.2 FPS)
Game Thread: 12.50 ms
Active NPCs: 50
NPC Update Time: 6.20 ms
Memory Usage: 2847.3 MB
```

### **Step 5: Find Results**
- **Console Output**: Immediate feedback in UE Editor console
- **CSV Files**: `Saved/Profiling/NPCPerformanceReport_*.csv`
- **Log Files**: `Saved/Logs/` for detailed debug info

## 📊 **What We're Looking For**

### **Baseline Performance (No NPCs)**
- **Target**: 60+ FPS (16.67ms frame time)
- **Expected**: Should easily hit 60 FPS

### **Light Load (50 NPCs)**  
- **Target**: 60 FPS maintained
- **Expected**: May drop to ~45-55 FPS

### **Medium Load (100 NPCs)**
- **Target**: 50+ FPS (20ms frame time)
- **Expected**: Likely ~30-40 FPS

### **Heavy Load (200 NPCs)**
- **Target**: 30+ FPS (33ms frame time)  
- **Expected**: Likely ~20-25 FPS 🚨

## 🔍 **Troubleshooting**

### **If Console Commands Don't Work:**
```bash
# Check if profiler subsystem loaded
stat memory

# Manual profiler access
UNPCPerformanceProfiler.StartProfiling 50 30
```

### **If Build Fails:**
1. Check that all new files were saved properly
2. Verify `DarkAge.Build.cs` was updated with dependencies
3. Try regenerating project files:
   - Right-click `DarkAge.uproject` 
   - "Generate Visual Studio project files"
   - Rebuild

### **If NPCs Don't Spawn:**
- Try testing in `ThirdPersonMap` 
- Ensure player character is present
- Check console for spawn errors

## 📈 **Success Criteria**

### **✅ Phase 1 Success:**
- Profiler loads without errors
- Console commands respond
- Performance data is collected
- CSV files are generated
- Frame time accurately reflects NPC load

### **📊 Data Collection Goals:**
- Baseline: 0 NPCs performance
- Light: 50 NPCs performance  
- Medium: 100 NPCs performance
- Heavy: 200+ NPCs performance

### **🎯 Analysis Targets:**
- Identify exact frame time cost per NPC
- Measure memory growth per NPC
- Determine NPC count where FPS drops below 60
- Confirm NPCs are the primary bottleneck

---

## **Next Steps After Data Collection:**

Once we have the diagnostic data, we'll:

1. **Analyze Results** - Confirm NPC tick cost hypothesis
2. **Design Phase 2** - Targeted optimizations based on data
3. **Implement LOD System** - Distance-based NPC update frequency
4. **Validate Improvements** - Re-run tests to measure gains

**Let me know the results of the performance tests once the build completes!**