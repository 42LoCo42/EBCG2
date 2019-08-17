echo "Installing CursedCat..."
git clone https://github.com/42LoCo42/libcursedcat.git
cd libcursedcat
make
sudo make install
cd ..

echo "Installing MSS..."
git clone https://github.com/42LoCo42/libmss.git
cd libmss
make
sudo make install
cd ..

echo "Creating EBCG2..."
make
make clean
rm -rf libcursedcat
rm -rf libmss
