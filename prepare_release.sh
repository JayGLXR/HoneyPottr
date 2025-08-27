#!/bin/bash
# HoneyPottr Release Preparation Script

echo "🍯 HoneyPottr Release Preparation"
echo "================================"

# Check if we're in the right directory
if [ ! -f "README.md" ] || [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: Run this script from the HoneyPottr project root directory"
    exit 1
fi

echo "✅ Project structure verified"

# Check if installer exists
if [ ! -f "HoneyPottr Installer.zip" ]; then
    echo "❌ Error: HoneyPottr Installer.zip not found"
    echo "   Expected at: $(pwd)/HoneyPottr Installer.zip"
    exit 1
fi

echo "✅ Installer package found ($(ls -lh "HoneyPottr Installer.zip" | awk '{print $5}'))"

# Initialize git if not already done
if [ ! -d ".git" ]; then
    echo "🔧 Initializing Git repository..."
    git init
    echo "✅ Git repository initialized"
else
    echo "✅ Git repository already exists"
fi

# Add all files
echo "📁 Adding files to Git..."
git add .

# Check git status
echo "📊 Git status:"
git status --short

# Create initial commit if needed
if ! git rev-parse --verify HEAD >/dev/null 2>&1; then
    echo "💾 Creating initial commit..."
    git commit -m "Initial release: HoneyPottr v1.0.0

- Complete Windows honeypot artifact generator
- Professional GUI installer with custom icon  
- Background service operation
- MIT License with Calvexa Group, LLC attribution
- Comprehensive documentation and legal compliance"
    echo "✅ Initial commit created"
else
    echo "✅ Repository already has commits"
fi

echo ""
echo "🚀 Repository Ready for GitHub!"
echo "================================"
echo ""
echo "Next steps:"
echo "1. Create repository on GitHub: https://github.com/new"
echo "2. Repository name: HoneyPottr"
echo "3. Description: Lightweight honeypot artifact generator that deceives malware into self-termination"
echo "4. Set as Public repository"
echo "5. Run these commands:"
echo ""
echo "   git remote add origin https://github.com/JayGLXR/HoneyPottr.git"
echo "   git branch -M main"
echo "   git push -u origin main"
echo ""
echo "6. Create release with 'HoneyPottr Installer.zip' as asset"
echo ""
echo "📚 See GITHUB_RELEASE_GUIDE.md for detailed instructions"
