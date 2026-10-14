#!/usr/bin/env python3
import os

icons = {}

# 1. Main Application Icon (Cutefish Task Manager / System Monitor)
icons["cutefish-taskmanager.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">
  <defs>
    <linearGradient id="bg" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop offset="0%" stop-color="#222B45"/>
      <stop offset="100%" stop-color="#121726"/>
    </linearGradient>
    <linearGradient id="pulseGrad" x1="0%" y1="0%" x2="100%" y2="0%">
      <stop offset="0%" stop-color="#00C9FF"/>
      <stop offset="50%" stop-color="#92FE9D"/>
      <stop offset="100%" stop-color="#00E5FF"/>
    </linearGradient>
    <linearGradient id="areaGrad" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop offset="0%" stop-color="#00E5FF" stop-opacity="0.35"/>
      <stop offset="100%" stop-color="#00E5FF" stop-opacity="0.0"/>
    </linearGradient>
    <linearGradient id="topBorder" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop offset="0%" stop-color="#FFFFFF" stop-opacity="0.3"/>
      <stop offset="100%" stop-color="#FFFFFF" stop-opacity="0.05"/>
    </linearGradient>
    <filter id="shadow" x="-10%" y="-10%" width="120%" height="120%">
      <feDropShadow dx="0" dy="3" stdDeviation="2.5" flood-color="#000000" flood-opacity="0.35"/>
    </filter>
    <filter id="glow" x="-20%" y="-20%" width="140%" height="140%">
      <feGaussianBlur stdDeviation="1.2" result="blur"/>
      <feComposite in="SourceGraphic" in2="blur" operator="over"/>
    </filter>
  </defs>

  <!-- Squircle Base with Drop Shadow -->
  <rect x="4" y="4" width="56" height="56" rx="14" ry="14" fill="url(#bg)" filter="url(#shadow)"/>
  
  <!-- Subtle Glass Border -->
  <rect x="4.5" y="4.5" width="55" height="55" rx="13.5" ry="13.5" fill="none" stroke="url(#topBorder)" stroke-width="1"/>

  <!-- Background Monitor Grid -->
  <g stroke="#00E5FF" stroke-opacity="0.08" stroke-width="1" stroke-dasharray="2 3">
    <line x1="10" y1="18" x2="54" y2="18"/>
    <line x1="10" y1="28" x2="54" y2="28"/>
    <line x1="10" y1="38" x2="54" y2="38"/>
    <line x1="10" y1="48" x2="54" y2="48"/>
    <line x1="20" y1="12" x2="20" y2="52"/>
    <line x1="32" y1="12" x2="32" y2="52"/>
    <line x1="44" y1="12" x2="44" y2="52"/>
  </g>

  <!-- Pulse Area Fill -->
  <path d="M 10 38 L 20 38 L 24 33 L 28 44 L 34 18 L 39 46 L 43 35 L 47 38 L 54 38 L 54 50 L 10 50 Z" fill="url(#areaGrad)"/>

  <!-- Main Pulse Line with Glow -->
  <path d="M 10 38 L 20 38 L 24 33 L 28 44 L 34 18 L 39 46 L 43 35 L 47 38 L 54 38" 
        fill="none" stroke="url(#pulseGrad)" stroke-width="2.8" stroke-linecap="round" stroke-linejoin="round" filter="url(#glow)"/>

  <!-- Live Pulse Dot -->
  <circle cx="34" cy="18" r="2.8" fill="#FFFFFF" filter="url(#glow)"/>
  <circle cx="34" cy="18" r="1.4" fill="#00E5FF"/>

  <!-- Top Glass Highlight Overlay -->
  <path d="M 5 18 C 5 10 10 5 18 5 L 46 5 C 54 5 59 10 59 18 C 59 21 44 23 32 23 C 18 23 5 21 5 18 Z" fill="#FFFFFF" opacity="0.08"/>
</svg>"""

# 2. Tab: Processes Icon
icons["tab-processes.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <defs>
    <linearGradient id="procGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#007AFF"/>
      <stop offset="100%" stop-color="#00C6FF"/>
    </linearGradient>
  </defs>
  <!-- Row 1 -->
  <rect x="2.5" y="3" width="19" height="4.5" rx="2.25" fill="url(#procGrad)"/>
  <circle cx="5" cy="5.25" r="1.2" fill="#FFFFFF"/>
  <rect x="7.5" y="4.5" width="7" height="1.5" rx="0.75" fill="#FFFFFF" opacity="0.9"/>
  <rect x="17" y="4.5" width="3" height="1.5" rx="0.75" fill="#FFFFFF" opacity="0.75"/>

  <!-- Row 2 -->
  <rect x="2.5" y="9.75" width="19" height="4.5" rx="2.25" fill="#8E8E93" opacity="0.25"/>
  <circle cx="5" cy="12" r="1.2" fill="#007AFF"/>
  <rect x="7.5" y="11.25" width="9" height="1.5" rx="0.75" fill="#8E8E93" opacity="0.8"/>
  <rect x="18" y="11.25" width="2" height="1.5" rx="0.75" fill="#8E8E93" opacity="0.6"/>

  <!-- Row 3 -->
  <rect x="2.5" y="16.5" width="19" height="4.5" rx="2.25" fill="#8E8E93" opacity="0.25"/>
  <circle cx="5" cy="18.75" r="1.2" fill="#34C759"/>
  <rect x="7.5" y="18" width="6" height="1.5" rx="0.75" fill="#8E8E93" opacity="0.8"/>
  <rect x="16" y="18" width="4" height="1.5" rx="0.75" fill="#8E8E93" opacity="0.6"/>
</svg>"""

# 3. Tab: Resources (Charts / Performance)
icons["tab-resources.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <defs>
    <linearGradient id="resGrad" x1="0%" y1="0%" x2="100%" y2="0%">
      <stop offset="0%" stop-color="#34C759"/>
      <stop offset="100%" stop-color="#30D158"/>
    </linearGradient>
  </defs>
  <!-- Chart Card Frame -->
  <rect x="2.5" y="2.5" width="19" height="19" rx="4" fill="none" stroke="#8E8E93" stroke-width="1.5" opacity="0.4"/>
  <!-- Grid Lines -->
  <line x1="3" y1="8.5" x2="21" y2="8.5" stroke="#8E8E93" stroke-width="1" stroke-dasharray="1.5 2" opacity="0.3"/>
  <line x1="3" y1="14.5" x2="21" y2="14.5" stroke="#8E8E93" stroke-width="1" stroke-dasharray="1.5 2" opacity="0.3"/>
  <!-- Area Wave -->
  <path d="M 3 17 L 6 15 L 9 17 L 12 11 L 15 13 L 18 6 L 21 9 L 21 21 L 3 21 Z" fill="url(#resGrad)" opacity="0.2"/>
  <!-- Wave Line -->
  <path d="M 3 17 L 6 15 L 9 17 L 12 11 L 15 13 L 18 6 L 21 9" fill="none" stroke="url(#resGrad)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
  <circle cx="18" cy="6" r="1.8" fill="#34C759"/>
</svg>"""

# 4. Tab: File Systems / Storage
icons["tab-storage.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <defs>
    <linearGradient id="diskGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#FF9500"/>
      <stop offset="100%" stop-color="#FF5E3A"/>
    </linearGradient>
  </defs>
  <!-- Drive 1 (Top) -->
  <rect x="2.5" y="3" width="19" height="7.5" rx="3" fill="#8E8E93" opacity="0.18"/>
  <rect x="2.5" y="3" width="19" height="7.5" rx="3" fill="none" stroke="#8E8E93" stroke-width="1.2" opacity="0.4"/>
  <circle cx="6" cy="6.75" r="1.3" fill="#34C759"/>
  <rect x="9" y="6" width="7" height="1.5" rx="0.75" fill="#8E8E93" opacity="0.7"/>
  <circle cx="18.5" cy="6.75" r="1" fill="#8E8E93" opacity="0.6"/>

  <!-- Drive 2 (Bottom) -->
  <rect x="2.5" y="13.5" width="19" height="7.5" rx="3" fill="url(#diskGrad)" opacity="0.9"/>
  <circle cx="6" cy="17.25" r="1.3" fill="#FFFFFF"/>
  <rect x="9" y="16.5" width="7" height="1.5" rx="0.75" fill="#FFFFFF" opacity="0.9"/>
  <circle cx="18.5" cy="17.25" r="1" fill="#FFFFFF" opacity="0.7"/>
</svg>"""

# 5. Tab: System Overview (Chip / Machine)
icons["tab-system.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <defs>
    <linearGradient id="chipGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#AF52DE"/>
      <stop offset="100%" stop-color="#5856D6"/>
    </linearGradient>
  </defs>
  <!-- Chip Pins -->
  <g stroke="#AF52DE" stroke-width="1.5" stroke-linecap="round">
    <line x1="7" y1="2" x2="7" y2="4.5"/>
    <line x1="12" y1="2" x2="12" y2="4.5"/>
    <line x1="17" y1="2" x2="17" y2="4.5"/>
    
    <line x1="7" y1="19.5" x2="7" y2="22"/>
    <line x1="12" y1="19.5" x2="12" y2="22"/>
    <line x1="17" y1="19.5" x2="17" y2="22"/>

    <line x1="2" y1="7" x2="4.5" y2="7"/>
    <line x1="2" y1="12" x2="4.5" y2="12"/>
    <line x1="2" y1="17" x2="4.5" y2="17"/>

    <line x1="19.5" y1="7" x2="22" y2="7"/>
    <line x1="19.5" y1="12" x2="22" y2="12"/>
    <line x1="19.5" y1="17" x2="22" y2="17"/>
  </g>
  <!-- Silicon Package Body -->
  <rect x="4.5" y="4.5" width="15" height="15" rx="3.5" fill="url(#chipGrad)"/>
  <!-- Silicon Die Core -->
  <rect x="8.5" y="8.5" width="7" height="7" rx="1.5" fill="#FFFFFF" opacity="0.3"/>
  <rect x="10" y="10" width="4" height="4" rx="1" fill="#FFFFFF"/>
</svg>"""

# 6. Action: End Process (Friendly Terminate)
icons["action-end.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <circle cx="12" cy="12" r="9.5" fill="#FF3B30" opacity="0.15"/>
  <circle cx="12" cy="12" r="9.5" fill="none" stroke="#FF3B30" stroke-width="1.5"/>
  <line x1="8.5" y1="8.5" x2="15.5" y2="15.5" stroke="#FF3B30" stroke-width="2" stroke-linecap="round"/>
  <line x1="15.5" y1="8.5" x2="8.5" y2="15.5" stroke="#FF3B30" stroke-width="2" stroke-linecap="round"/>
</svg>"""

# 7. Action: Force Kill Process
icons["action-kill.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <defs>
    <linearGradient id="killGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#FF3B30"/>
      <stop offset="100%" stop-color="#D70015"/>
    </linearGradient>
  </defs>
  <!-- Shield/Hexagon Badge -->
  <path d="M 12 2.5 L 19.5 6 L 19.5 13 C 19.5 17.5 16 20.8 12 22 C 8 20.8 4.5 17.5 4.5 13 L 4.5 6 Z" fill="url(#killGrad)"/>
  <!-- Lightning Strike -->
  <path d="M 12.5 6.5 L 8.5 12.5 L 11.5 12.5 L 10.5 17.5 L 15.5 11.5 L 12.5 11.5 Z" fill="#FFFFFF"/>
</svg>"""

# 8. Action: Pause Process (SIGSTOP)
icons["action-pause.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <circle cx="12" cy="12" r="9.5" fill="#FF9500" opacity="0.15"/>
  <circle cx="12" cy="12" r="9.5" fill="none" stroke="#FF9500" stroke-width="1.5"/>
  <rect x="8.5" y="7.5" width="2.5" height="9" rx="1.2" fill="#FF9500"/>
  <rect x="13" y="7.5" width="2.5" height="9" rx="1.2" fill="#FF9500"/>
</svg>"""

# 9. Action: Resume Process (SIGCONT)
icons["action-resume.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <circle cx="12" cy="12" r="9.5" fill="#34C759" opacity="0.15"/>
  <circle cx="12" cy="12" r="9.5" fill="none" stroke="#34C759" stroke-width="1.5"/>
  <path d="M 9.5 7.5 C 9.5 6.8 10.3 6.4 10.9 6.8 L 16.5 10.8 C 17 11.2 17 11.8 16.5 12.2 L 10.9 16.2 C 10.3 16.6 9.5 16.2 9.5 15.5 Z" fill="#34C759"/>
</svg>"""

# 10. Action: Priority / Nice
icons["action-priority.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <!-- Gauge Arc -->
  <path d="M 4.5 17 C 3.5 14.5 3.8 11.5 5.5 9 C 8.5 4.5 14.5 4 18 7 C 19.8 8.8 20.6 11.3 20.2 14" fill="none" stroke="#AF52DE" stroke-width="2" stroke-linecap="round"/>
  <!-- Needle -->
  <line x1="12" y1="15" x2="16.5" y2="8.5" stroke="#AF52DE" stroke-width="2.2" stroke-linecap="round"/>
  <circle cx="12" cy="15" r="2.5" fill="#AF52DE"/>
</svg>"""

# 11. Action: Details / Inspector
icons["action-details.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <circle cx="12" cy="12" r="9.5" fill="none" stroke="#007AFF" stroke-width="1.8"/>
  <circle cx="12" cy="7.5" r="1.3" fill="#007AFF"/>
  <rect x="11" y="10.5" width="2" height="6.5" rx="1" fill="#007AFF"/>
</svg>"""

# 12. Metric: CPU
icons["metric-cpu.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <rect x="5" y="5" width="14" height="14" rx="3" fill="#007AFF" opacity="0.15"/>
  <rect x="5" y="5" width="14" height="14" rx="3" fill="none" stroke="#007AFF" stroke-width="1.8"/>
  <rect x="8.5" y="8.5" width="7" height="7" rx="1.5" fill="#007AFF"/>
  <!-- Pins -->
  <line x1="8" y1="2" x2="8" y2="4.5" stroke="#007AFF" stroke-width="1.5" stroke-linecap="round"/>
  <line x1="12" y1="2" x2="12" y2="4.5" stroke="#007AFF" stroke-width="1.5" stroke-linecap="round"/>
  <line x1="16" y1="2" x2="16" y2="4.5" stroke="#007AFF" stroke-width="1.5" stroke-linecap="round"/>
  <line x1="8" y1="19.5" x2="8" y2="22" stroke="#007AFF" stroke-width="1.5" stroke-linecap="round"/>
  <line x1="12" y1="19.5" x2="12" y2="22" stroke="#007AFF" stroke-width="1.5" stroke-linecap="round"/>
  <line x1="16" y1="19.5" x2="16" y2="22" stroke="#007AFF" stroke-width="1.5" stroke-linecap="round"/>
</svg>"""

# 13. Metric: Memory (RAM Stick)
icons["metric-memory.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <!-- DIMM PCB -->
  <rect x="3" y="6" width="18" height="12" rx="2" fill="#AF52DE" opacity="0.15"/>
  <rect x="3" y="6" width="18" height="12" rx="2" fill="none" stroke="#AF52DE" stroke-width="1.8"/>
  <!-- Memory Chips -->
  <rect x="5.5" y="8.5" width="3" height="4.5" rx="0.8" fill="#AF52DE"/>
  <rect x="10.5" y="8.5" width="3" height="4.5" rx="0.8" fill="#AF52DE"/>
  <rect x="15.5" y="8.5" width="3" height="4.5" rx="0.8" fill="#AF52DE"/>
  <!-- Gold Pins -->
  <line x1="4.5" y1="16.5" x2="6.5" y2="16.5" stroke="#AF52DE" stroke-width="1.5"/>
  <line x1="8" y1="16.5" x2="11" y2="16.5" stroke="#AF52DE" stroke-width="1.5"/>
  <line x1="13" y1="16.5" x2="16" y2="16.5" stroke="#AF52DE" stroke-width="1.5"/>
  <line x1="17.5" y1="16.5" x2="19.5" y2="16.5" stroke="#AF52DE" stroke-width="1.5"/>
</svg>"""

# 14. Metric: Network (Transfer Arrows)
icons["metric-network.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <!-- Down Arrow (Download) -->
  <path d="M 8 4 L 8 16 M 4.5 12.5 L 8 16.5 L 11.5 12.5" fill="none" stroke="#34C759" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round"/>
  <!-- Up Arrow (Upload) -->
  <path d="M 16 20 L 16 8 M 12.5 11.5 L 16 7.5 L 19.5 11.5" fill="none" stroke="#007AFF" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round"/>
</svg>"""

# 15. Metric: Disk
icons["metric-disk.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <rect x="3" y="5" width="18" height="14" rx="3" fill="#FF9500" opacity="0.15"/>
  <rect x="3" y="5" width="18" height="14" rx="3" fill="none" stroke="#FF9500" stroke-width="1.8"/>
  <line x1="3" y1="14" x2="21" y2="14" stroke="#FF9500" stroke-width="1.5"/>
  <circle cx="6.5" cy="16.5" r="1.2" fill="#FF9500"/>
  <circle cx="10" cy="16.5" r="1.2" fill="#FF9500"/>
  <rect x="14" y="15.5" width="4.5" height="2" rx="0.8" fill="#FF9500"/>
</svg>"""

# 16. Process Default App Icon
icons["process-default.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <rect x="3" y="3" width="18" height="18" rx="4.5" fill="#007AFF" opacity="0.15"/>
  <rect x="3" y="3" width="18" height="18" rx="4.5" fill="none" stroke="#007AFF" stroke-width="1.5"/>
  <circle cx="8" cy="8" r="1.5" fill="#007AFF"/>
  <line x1="12" y1="8" x2="16" y2="8" stroke="#007AFF" stroke-width="1.8" stroke-linecap="round"/>
  <line x1="8" y1="12" x2="16" y2="12" stroke="#007AFF" stroke-width="1.8" stroke-linecap="round"/>
  <line x1="8" y1="16" x2="14" y2="16" stroke="#007AFF" stroke-width="1.8" stroke-linecap="round"/>
</svg>"""

# 17. Process System / Daemon Icon
icons["process-system.svg"] = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
  <circle cx="12" cy="12" r="8.5" fill="#8E8E93" opacity="0.15"/>
  <circle cx="12" cy="12" r="8.5" fill="none" stroke="#8E8E93" stroke-width="1.5"/>
  <!-- Gear Teeth / Core -->
  <circle cx="12" cy="12" r="3.5" fill="none" stroke="#8E8E93" stroke-width="2"/>
  <line x1="12" y1="5.5" x2="12" y2="7.5" stroke="#8E8E93" stroke-width="2" stroke-linecap="round"/>
  <line x1="12" y1="16.5" x2="12" y2="18.5" stroke="#8E8E93" stroke-width="2" stroke-linecap="round"/>
  <line x1="5.5" y1="12" x2="7.5" y2="12" stroke="#8E8E93" stroke-width="2" stroke-linecap="round"/>
  <line x1="16.5" y1="12" x2="18.5" y2="12" stroke="#8E8E93" stroke-width="2" stroke-linecap="round"/>
</svg>"""

for name, svg_content in icons.items():
    path = os.path.join("icons", name)
    with open(path, "w", encoding="utf-8") as f:
        f.write(svg_content.strip() + "\n")
    print(f"Generated {path}")
