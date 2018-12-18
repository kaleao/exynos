all:
	doxygen exynos_repo.doxyfile && cp -r custom/images output/latex && cd output/latex && make
	mkdir -p output/pdf && cp output/latex/refman.pdf output/pdf/exynos_manual.pdf

clean:
	rm -rf output
