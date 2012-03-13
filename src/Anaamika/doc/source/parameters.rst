**************************************
Alphabetical listing of all parameters
**************************************

.. glossary::
    :sorted:
    
    filename
        This parameter is a string (no default) that sets the input image file name. The input image can be a FITS or CASA 2-, 3-, or 4-D cube.
        
    advanced_opts
        This parameter is a Boolean (default is ``False``). If ``True``, the advanced options are shown. See :ref:`advanced_opts` for details of the advanced options.
        
    atrous_do
        This parameter is a Boolean (default is ``False``). If ``True``, wavelet decomposition will be performed. See :ref:`atrous_do` for details of the options.

    beam
        This parameter is a tuple (default is ``None``) that defines the FWHM of restoring beam. Specify as (maj, min, pos ang E of N) in degrees. E.g., ``beam = (0.06, 0.02, 13.3)``. For more than one channel, use the ``beam_spectrum`` parameter. If the beam is not given by the user, then it is looked for in the image header. If not found, then an error is raised. PyBDSM will not work without knowledge of the restoring beam.

    flagging_opts
        This parameter is a Boolean (default is ``False``). If ``True``, the Gaussian flagging options will be listed. See :ref:`flagging_opts` for details of the options.

    frequency
        This parameter is a float (default is ``None``) that defines the frequency in Hz of the input image. E.g., ``frequency = 74e6``. For more than one channel, use the :term:`frequency_sp` parameter. If the frequency is not given by the user, then it is looked for in the image header. If not found, then an error is raised. PyBDSM will not work without knowledge of the frequency.

    interactive
        This parameter is a Boolean (default is ``False``). If ``True``, interactive mode is used. In interactive mode, plots are displayed at various stages of the processing so that the user may check the progress of the fit.

        First, plots of the rms and mean background images are displayed along with the islands found, before fitting of Gaussians takes place. The user should verify that the islands and maps are reasonable before preceding.

        Next, if ``atrous_do = True``, the fits to each wavelet scale are shown. The wavelet fitting may be truncated at the current scale if desired.

        Lastly, the final results are shown.

    mean_map
        This parameter is a string (default is ``'default'``) that determines how the background mean map is computed and
        how it is used further.
    
        If ``'const'``\, then the value of the clipped mean of the entire image (set
        by the ``kappa_clip`` option) is used as the background mean map.
    
        If ``'zero'``\, then a value of zero is used.
    
        If ``'map'``\, then the 2-dimensional mean map is computed and used. The
        resulting mean map is largely determined by the value of the ``rms_box``
        parameter (see the ``rms_box`` parameter for more information).
    
        If ``'default'``\, then PyBDSM will attempt to determine automatically
        whether to use a 2-dimensional map or a constant one as follows. First,
        the image is assumed to be confused if ``bmpersrc_th`` < 25 or the ratio of
        the clipped mean to rms (clipped mean/clipped rms) is > 0.1, else the
        image is not confused. Next, the mean map is checked to see if its
        spatial variation is significant. If so, then a 2-D map is used and, if
        not, then the mean map is set to either 0.0 or a constant depending on
        whether the image is thought to be confused or not.
    
        Generally, ``'default'`` works well. However, if there is significant
        extended emission in the image, it is often necessary to force the use
        of a constant mean map using either ``'const'`` or ``'mean'``\.

    multichan_opts
        This parameter is a Boolean (default is ``False``). If ``True``, the multichannel options will be listed. See :ref:`multichan_opts` for details of the options.

    output_opts
        This parameter is a Boolean (default is ``False``). If ``True``, the output options will be listed. See :ref:`output_opts` for details of the options.

    polarisation_do
        This parameter is a Boolean (default is ``False``). If ``True``, polarization properties will be calculated for the sources. See :ref:`polarisation_do` for details of the options.

    psf_vary_do
        This parameter is a Boolean (default is ``False``). If ``True``, the spatial variation of the PSF will be estimated and its effects corrected. See :ref:`psf_vary_do` for details of the options.

    rms_box
        This parameter is a tuple (default is ``None``) of two integers and is probably the most important input
        parameter for PyBDSM. The first integer, boxsize, is the size of the 2-D
        sliding box for calculating the rms and mean over the entire image. The
        second, stepsize, is the number of pixels by which this box is moved for
        the next measurement. If ``None``\, then suitable values are calculated
        internally.
        
        In general, it is best to choose a box size that corresponds to the
        typical scale of artifacts in the image, such as those that are common
        around bright sources. Too small of a box size will effectively raise
        the local rms near a source so much that a source may not be fit at all;
        too large a box size can result in underestimates of the rms due to
        oversmoothing. A step size of 1/3 to 1/4 of the box size usually works
        well.

    rms_map
        This parameter is a Boolean (default is ``None``). If ``True``\, then the 2-D background rms image is computed and used. If
        ``False``\, then a constant value is assumed (use ``rms_value`` to force the rms
        to a specific value). If ``None``\, then the 2-D rms image is calculated, and
        if the variation is statistically significant then it is taken, else a
        constant value is assumed. The rms image used for each channel in
        computing the spectral index follows what was done for the
        channel-collapsed image.
        
        Generally, the default value works well. However, if there is significant extended
        emission in the image, it is often necessary to force the use of a
        constant rms map by setting ``rms_map = False``.

    shapelet_do
        This parameter is a Boolean (default is ``False``). If ``True``, shapelet decomposition of the islands will be performed. See :ref:`shapelet_do` for details of the options.

    spectralindex_do
        This parameter is a Boolean (default is ``False``). If ``True``, spectral indices will be calculated for the sources. See :ref:`spectralindex_do` for details of the options.

    thresh
        This parameter is a string (default is ``None``). If ``thresh = 'hard'``\, then a hard threshold is assumed, given by
        thresh_pix. If ``thresh = 'fdr'``\, then the False Detection Rate algorithm
        of Hopkins et al. (2002) is used to calculate the value of ``thresh_pix``\.
        If ``thresh = None``\, then the false detection probability is first
        calculated, and if the number of false source pixels is more than
        ``fdr_ratio`` times the estimated number of true source pixels, then the
        ``'fdr'`` threshold option is chosen, else the ``'hard'`` threshold option is
        chosen.

    thresh_isl
        This parameter is a float (default is 3.0) that determines the region to which fitting is done. A higher
        value will produce smaller islands, and hence smaller regions that are
        considered in the fits. A lower value will produce larger islands. Use
        the thresh_pix parameter to set the detection threshold for sources.
        Generally, ``thresh_isl`` should be lower than ``thresh_pix``\.
        
        Only regions above the absolute threshold will be used. The absolute
        threshold is calculated as ``abs_thr = mean + thresh_isl * rms``\. Use the
        ``mean_map`` and ``rms_map`` parameters to control the way the mean and rms are
        determined.

    thresh_pix
        This parameter is a float (default is 5.0) that sets the source detection threshold in number of
        sigma above the mean. If false detection rate thresholding is used, this
        value is ignored and ``thresh_pix`` is calculated inside the program
        
        This parameter sets the overall detection threshold for islands (i.e.
        ``thresh_pix = 5`` will find all sources with peak fluxes of 5-sigma or
        greater). Use the ``thresh_isl`` parameter to control how much of each
        island is used in fitting. Generally, ``thresh_pix`` should be larger than
        ``thresh_isl``.
        
        Only islands with peaks above the absolute threshold will be used. The
        absolute threshold is calculated as ``abs_thr = mean + thresh_pix * rms``\.
        Use the ``mean_map`` and ``rms_map`` parameters to control the way the mean and
        rms are determined.

    ch0_flagged
        This parameter is a Boolean (default is ``False``) that determines whether to plot the ch0 image (the image used for source detection) with any flagged Gaussians overplotted.
        
    ch0_image
        This parameter is a Boolean (default is ``True``) that determines whether to plot the ch0 image (the image used for source detection).
             
    ch0_islands 
        This parameter is a Boolean (default is ``True``) that determines whether to plot the ch0 image (the image used for source detection) with islands and Gaussians overplotted.
        
    gmodel_image
        This parameter is a Boolean (default is ``True``) that determines whether to plot the Gaussian model image.
        
    gresid_image
        This parameter is a Boolean (default is ``True``) that determines whether to plot the Gaussian residual image.
        
    mean_image
        This parameter is a Boolean (default is ``True``) that determines whether to plot the background mean image.
        
    pyramid_srcs
        This parameter is a Boolean (default is ``False``) that determines whether to plot the pyramidal sources constructed during wavelet decomposition.
        
    rms_image
        This parameter is a Boolean (default is ``True``) that determines whether to plot the background rms image.
        
    smodel_image
        This parameter is a Boolean (default is ``False``) that determines whether to plot the shapelet model image.
    
    source_seds 
        This parameter is a Boolean (default is ``False``) that determines whether to plot the source SEDs and best-fit spectral indices. 
        
    sresid_image
        This parameter is a Boolean (default is ``False``) that determines whether to plot the shapelet residual image.
        
    clobber
        This parameter is a Boolean (default is ``False``) that determines whether existing files are overwritten or not.

    img_format
        This parameter is a string (default is ``'fits'``) that sets the output file format: ``'fits'`` - FITS format, ``'casa'`` - CASA format.
        
    img_type
        This parameter is a string (default is ``'gaus_resid'``) that sets the type of image to export.
        The following images can be exported:
        
        * ``'ch0'`` - image used for source detection
        
        * ``'rms'`` - rms map image
        
        * ``'mean'`` - mean map image
        
        * ``'gaus_resid'`` - Gaussian model residual image
        
        * ``'gaus_model'`` - Gaussian model image
        
        * ``'shap_resid'`` - Shapelet model residual image
        
        * ``'shap_model'`` - Shapelet model image

    blank_zeros
        This parameter is a Boolean (default is ``False``). If ``True``, all pixels in the input image with values of 0.0 are blanked. If ``False``, any such pixels are left unblanked (and hence will affect the rms and mean maps, etc.). Pixels with a value of NaN are always blanked.
        
    bmpersrc_th
        This parameter is a float (default is ``None``) that sets the theoretical estimate of number of beams per source.
        If ``None``, the value is calculated
        as N/[n*(alpha-1)], where N is the total number of pixels in the image,
        n is the number of pixels in the image whose value is greater than 5
        times the clipped rms, and alpha is the slope of the differential source
        counts distribution, assumed to be 2.5. 
        
        The value of ``bmpersrc_th`` is used
        to estimate the average separation in pixels between two sources, which
        in turn is used to estimate the boxsize for calculating the background
        rms and mean images. In addition, if the value is below 25 (or the ratio
        of clipped mean to clipped rms of the image is greater than 0.1), the
        image is assumed to be confused and hence the background mean is put to
        zero.
        
    check_outsideuniv
        This parameter is a Boolean (default is ``False``). If ``True``, then the coordinate of each pixel is examined to check if it is
        outside the universe, which may happen when, e.g., an all sky image is
        made with SIN projection (commonly done at LOFAR earlier). When found,
        these pixels are blanked (since imaging software do not do this on their
        own). Note that this process takes a lot of time, as every pixel is
        checked in case weird geometries and projections are used.
        
    fdr_alpha
        This parameter is a float (default is 0.05) that sets the value of alpha for the FDR algorithm for thresholding. If ``thresh`` is ``'fdr'``, then the estimate of ``fdr_alpha`` (see Hopkins et al. 2002 for details) is stored in this parameter.

    fdr_ratio
        This parameter is a float (default is 0.1). When ``thresh = None``, if #false_pix / #source_pix < fdr_ratio, ``thresh = 'hard'`` otherwise ``thresh = 'fdr'``.
    
    fittedimage_clip
        This parameter is a float (default is 0.1). When the residual image is being made after Gaussian decomposition, the
        model images for each fitted Gaussian are constructed up to a size 2b,
        such that the amplitude of the Gaussian falls to a value of
        ``fitted_image_clip`` times the local rms, b pixels from the peak.
        
    group_by_isl
        This parameter is a Boolean (default is ``False``). If True, all Gaussians in the island belong to a single source. If
        False, grouping is controlled by the group_tol parameter.
        
    group_tol
        This parameter is a float (default is 1.0) that sets the tolerance for grouping of Gaussians into sources: larger values will
        result in larger sources. Sources are created by grouping nearby Gaussians as follows: (1) If the
        minimum value between two Gaussians in an island is more than ``group_tol * thresh_isl * rms_clip``\, and (2) if the centres are seperated by a
        distance less than 0.5*``group_tol`` of the sum of their FWHMs along the PA
        of the line joining them, they belong to the same island.
        
    ini_gausfit
        This parameter is a string (default is ``'default'``). These are three different ways of estimating the initial guess for
        fitting of Gaussians to an island of emission. If ``'default'``, the maximum
        number of Gaussians is estimated from the number of peaks in the island.
        An initial guess is made for the parameters of these Gaussians before
        final fitting is done. This method should produce the best results when
        there are no large sources present. If ``'simple'``, the maximum number of
        Gaussians per island is set to 25, and no initial guess for the gaussian
        parameters is made. Lastly, the ``'nobeam'`` method is similar to the
        ``'default'`` method, but no information about the beam is used. This method
        is best used when source sizes are expected to be very different from
        the beam and is generally slower than the other methods. For wavelet
        images, the value used for the original image is used for wavelet order
        j <= 3 and ``'nobeam'`` for higher orders.
        
    kappa_clip
        This parameter is a float (default is 3.0) that is the factor used for Kappa-alpha clipping, as in
        AIPS. For an image with few source pixels added on to (Gaussian) noise
        pixels, the dispersion of the underlying noise will need to be
        determined. This is done iteratively, whereby the actual dispersion is
        first computed. Then, all pixels whose value exceeds kappa clip times
        this rms are excluded and the rms is computed again. This process is
        repeated until no more pixels are excluded. For well behaved noise
        statistics, this process will converge to the true noise rms with a
        value for this parameter ~3-5. A large fraction of source pixels, less
        number of pixels in total, or significant non-Gaussianity of the
        underlying noise will all lead to non-convergence.
        
    minpix_isl
        This parameter is an integer (default is ``None``) that sets the minimum number of pixels in an island
        for the island to be included. If
        ``None``\, the number of pixels is set to 1/3 of the area of an unresolved source
        using the beam and pixel size information in the image header. It is set
        to 4 pixels for all wavelet images.
        
    peak_fit
        This parameter is a Boolean (default is ``True``). When True, PyBDSM will first identify and fit peaks of emission in
        large islands (the size of islands for which peak fitting is done is
        controlled with the ``peak_maxsize`` option). Once the peaks have been fit,
        the residual emission is then fit in the normal way. Enabling this
        option will generally speed up fitting, but may result in somewhat
        higher residuals.
        
    peak_maxsize
        This parameter is a float (default is 30.0). If island size in beam area is more than this value, attempt to fit peaks
        separately (if ``peak_fit = True``). The minimum value is 30.
        
    rms_value
        This parameter is a float (default is ``None``) that sets the value of constant rms in Jy/beam to use if ``rms_map = False``. If ``None``, the value is 
        calculated inside the program.
        
    spline_rank
        This parameter is an integer (default is 3) that sets the order of the interpolating spline function
        to interpolate the background rms and mean maps over the entire image.
        
    split_isl
        This parameter is a Boolean (default is ``True``). If ``True``, an island is split if it is too large, has a large convex deficiency and it
        opens well. If it doesn't open well, then ``isl.mean = isl.clipped_mean``,
        and is taken for fitting. Splitting, if needed, is always done for
        wavelet images
        
    splitisl_frac_bigisl3
        This parameter is a float (default is 0.8) that sets the fraction of island area for 3x3 opening to be used. When deciding to split an island, if the largest sub island when opened
        with a 3x3 footprint is less than this fraction of the island area, then
        a 3x3 opening is considered.

    splitisl_maxsize
        This parameter is a float (default is 50.0). If island size in beam area is more than this, consider splitting
        island. The minimum value is 50.
        
    splitisl_size_extra5
        This parameter is a float (default is 0.1) that sets the fraction of the island area for 5x5 opening to be used.
        When deciding to split an island, if the smallest extra sub islands
        while opening with a 5x5 footprint add up to at least this fraction of
        the island area, and if the largest sub island is less than 75% the size
        of the largest when opened with a 3x3 footprint, a 5x5 opening is taken.
        
    stop_at
        This parameter is a string (default is ``None``) that stops an analysis after: 'isl' = island finding step or 'read' = image reading step.
       
    trim_box
        This parameter is a tuple (default is ``None``) that defines a subregion of the image on which to do source detection. It is specified as (xmin, xmax,
        ymin, ymax) in pixels. E.g., ``trim_box = (120, 840, 15, 895)``\. If ``None``, the entire image is used.

    flag_bordersize
        This parameter is an integer (default is 0). Any fitted Gaussian whose centre is ``flag_bordersize`` pixels outside the island
        bounding box is flagged. The flag value is increased by 4 (for x) and 8
        (for y).
        
    flag_maxsize_bm
        This parameter is a float (default is 50.0). Any fitted Gaussian whose size is greater than ``flag_maxsize_bm`` times the
        synthesized beam is flagged. The flag value is increased by 64.
    
    flag_maxsize_isl
        This parameter is a float (default is 1.0). Any fitted Gaussian whose maximum x-dimension is larger than
        ``flag_maxsize_isl`` times the x-dimension of the island (and likewise for
        the y-dimension) is flagged. The flag value is increased by 16 (for x)
        and 32 (for y).
    
    flag_maxsnr
        This parameter is a float (default is 1.5). Any fitted Gaussian whose peak is greater than ``flag_maxsnr`` times
        ``thresh_pix`` times the local rms is flagged. The flag value is increased
        by 2.
    
    flag_minsize_bm
        This parameter is a float (default is 0.7). If ``flag_smallsrc`` is True, then any fitted Gaussian whose size is less
        than ``flag_maxsize_bm`` times the synthesized beam is flagged. The Gaussian
        flag is increased by 128.
    
    flag_minsnr
        This parameter is a float (default is 0.7). Any fitted Gaussian whose peak is less than ``flag_minsnr`` times ``thresh_pix``
        times the local rms is flagged. The flag value is increased by 1.
    
    flag_smallsrc
        This parameter is a Boolean (default is ``False``). If ``True``\, then fitted Gaussians whose size is less than ``flag_minsize_bm``
        times the synthesized beam area are flagged.  When combining Gaussians
        into sources, an error is raised if a 2x2 box with the peak of the
        Gaussian does not have all four pixels belonging to the source. Usually
        this means that the Gaussian is an artifact or has a very small size. 

        If ``False``\, then if either of the sizes of the fitted Gaussian is zero,
        then the Gaussian is flagged.

        If the image is barely Nyquist sampled, this flag is best set to ``False``\.
        This flag is automatically set to ``False`` while decomposing wavelet images
        into Gaussians. 

    bbs_patches
        This parameter is a string (default is ``None``) that sets the type of patch to use in BBS-formatted catalogs. When the Gaussian catalogue is written as a BBS-readable sky file, this
        determines whether all Gaussians are in a single patch (``'single'``), there are no
        patches (``None``), all Gaussians for a given source are in a separate patch (``'source'``), or
        each Gaussian gets its own patch (``'gaussian'``).
        
        If you wish to have patches defined by island, then set
        ``group_by_isl = True`` before fitting to force all
        Gaussians in an island to be in a single source. Then set
        ``bbs_patches = 'source'`` when writing the catalog.
        
    indir
        This parameter is a string (default is ``None``) that sets the directory of input FITS files. If ``None``, the directory is defined by the input filename.
        
    opdir_overwrite
        This parameter is a string (default is ``'overwrite'``) that determines whether existing output files are overwritten or not.
        
    output_all
        This parameter is a Boolean (default is ``False``). If ``True``\, all output products are written automatically to the directory ``'filename_pybdsm'``.
        
    output_fbdsm
        This parameter is a Boolean (default is ``False``). If ``True``\, write out fBDSM format output files for use in Anaamika.
        
    plot_allgaus
        This parameter is a Boolean (default is ``False``). If ``True``\, make a plot of all Gaussians at the end.
    
    plot_islands
        This parameter is a Boolean (default is ``False``). If ``True``\, make separate plots of each island during fitting
        (for large images, this may take a long time and a
        lot of memory).
    
    plot_pyramid
        This parameter is a Boolean (default is ``False``). If ``True``\, make separate plots of each pyramid source during
        wavelet fitting .
        
    print_timing
        This parameter is a Boolean (default is ``False``). If ``True``\, print basic timing information.
    
    quiet
        This parameter is a Boolean (default is ``False``). If ``True``\, suppress text output to screen. Output is still
        sent to the log file as usual.
    
    savefits_meanim
        This parameter is a Boolean (default is ``False``). If ``True``\, save background mean image as a FITS file.
    
    savefits_normim
        This parameter is a Boolean (default is ``False``). If ``True``\, save norm image as a FITS file.

    savefits_rankim
        This parameter is a Boolean (default is ``False``). If ``True``\, save island rank image as a FITS file.

    savefits_residim
        This parameter is a Boolean (default is ``False``). If ``True``\, save residual image as a FITS file.

    savefits_rmsim
        This parameter is a Boolean (default is ``False``). If ``True``\, save background rms image as a FITS file.

    solnname
        This parameter is a string (default is ``None``) that sets the name of the run, to be appended to the name of the
        output directory.
        
    verbose_fitting
        This parameter is a Boolean (default is ``False``). If ``True``\, print out extra information during fitting.
      
    beam_sp_derive
        This parameter is a Boolean (default is ``False``). If `True` and the parameter beam_spectrum is ``None``, then we assume that the
        beam in the header is for the median frequency of the image cube and
        scale accordingly to calculate the beam per channel. If ``False``, then a
        constant value of the beam is taken instead.
               
    beam_spectrum 
        his parameter is a list of tuples (default is ``None``) that sets the FWHM of synthesized beam per channel. Specify as [(bmaj_ch1, bmin_ch1,
        bpa_ch1), (bmaj_ch2, bmin_ch2, bpa_ch2), etc.] in degrees. E.g.,
        ``beam_spectrum = [(0.01, 0.01, 45.0), (0.02, 0.01, 34.0)]`` for two
        channels.

        If ``None``, then the channel-dependent restoring beam is either assumed to
        be a constant or to scale with frequency, depending on whether the
        parameter ``beam_sp_derive`` is ``False`` or ``True``.
               
    collapse_av
        This parameter is a list of integers (default is ``[]``) that specifies the channels to be averaged to produce the
        continuum image for performing source extraction, if ``collapse_mode`` is
        ``'average'``. If the value is ``[]``, then all channels are used. Otherwise, the
        value is a Python list of channel numbers.
               
    collapse_ch0 
        This parameter is an integer (default is 0) that specifies the number of the channel for source extraction, if ``collapse_mode = 'single'``.
               
    collapse_mode
        This parameter is a string (default is ``'average'``) that determines whether, when multiple channels are present,
        the source extraction is done on a single channel (``'single'``) or an average of many
        channels (``'average'``).       
               
    collapse_wt
        This parameter is a string (default is ``'unity'``). When ``collapse_mode`` is ``'average'``, then if this value is ``'unity'``, the
        channels given by ``collapse_av`` are averaged with unit weights and if
        ``'rms'``, then they are averaged with weights which are inverse square of
        the clipped rms of each channel image.
               
    frequency_sp
        This parameter is a list of floats (default is ``None``) that sets the frequency in Hz of channels in input image when more than one channel is
        present. E.g., ``frequency_sp = [74e6, 153e6]``.
    
        If the frequency is not given by the user, then it is looked for in the
        image header. If not found, then an error is raised. PyBDSM will not
        work without the knowledge of the frequency.
    
    atrous_bdsm_do
        This parameter is a Boolean (default is ``False``). If ``True``\, PyBDSM performs source extraction on each wavelet scale. Unless this is set to ``True``, the image cannot be decomposed into a pyramidal set of sources for morphological transforms.
        
    atrous_jmax
        This parameter is an integer (default is 0) which is the maximum order of the *à trous* wavelet
        decomposition. If 0 (or <0 or >15), then the value is determined within
        the program. The value of this parameter is then estimated as the
        (lower) rounded off value of ln[(nm-l)/(l-1) + 1]/ln2 + 1 where nm is
        the minimum of the residual image size (n, m) in pixels and l is the
        length of the filter *à trous* lpf (see the ``atrous_lpf`` parameter for more
        info).
        
        A sensible value is such that the size of the kernel is not more than
        3-4 times smaller than the smallest image dimension.

    atrous_lpf
        This parameter is a string (default is ``'b3'``) that sets the low pass filter, which can currently be either the B3 spline
        or the triangle function, which is used to generate the *à trous*
        wavelets. The B3 spline is [1, 4, 6, 4, 1] and the triangle is [1, 2,
        1], normalised so that the sum is unity. The lengths of the filters are
        hence 5 and 3 respectively.
        
    atrous_orig_isl
        This parameter is a Boolean (default is ``True``). If ``True``\, Gaussians will only be fit to the wavelet images inside islands
        found in the original image. If ``False``\, wavelet Gaussians can be fit to
        any part of the wavelet image.

    psf_high_snr
        This parameter is a float (default is ``None``). Gaussians with SNR greater than this are used to determine the PSF
        variation, even if they are deemed to be resolved. This corrects for the
        unreliability at high SNRs in the algorithm used to find unresolved
        sources. The minimum value is 20.0. If ``None``, then no such selection is made.

    psf_itess_method
        This parameter is an integer (default is 0) which can be 0, 1, 2 or 3, which
        corresponds to a tessellation method. If 0, 2 or 3, then the weights
        used for Voronoi tessellation are unity, log(SNR) and sqrt[log(SNR)]
        where SNR is the signal to noise ratio of the generator in a tile. If 1,
        then the image is tessellated such that each tile has smooth boundaries
        instead of straight lines, using pixel-dependent weights.

    psf_kappa2
        This parameter is a float (default is 2.0). When iteratively arriving at a statistically probable set of
        'unresolved' sources, the fitted major and minor axis sizes versus SNR
        are binned and fitted with analytical functions. Those Gaussians which
        are within ``psf_kappa2`` times the fitted rms from the fitted median are
        then considered 'unresolved' and are used further to estimate the PSFs.
    
    psf_nsig
        This parameter is a float (default is 3.0). When constructing a set of 'unresolved' sources for psf estimation, the
        (clipped) median, rms and mean of major and minor axis sizes of
        Gaussians versus SNR within each bin is calculated using ``kappa = psf_nsig``.
    
    psf_over
        This parameter is an integer (default is 2). When constructing a set of 'unresolved' sources for psf estimation, this parameter controls the factor of nyquist sample for binning bmaj, etc. vs SNR.
    
    psf_snrcut
        This parameter is a float (default is 10.0). Only Gaussians with SNR greater than this are considered for processing.
        The minimum value is 5.0
    
    psf_snrcutstack
        This parameter is a float (default is 15.0). Only Gaussians with SNR greater than this are used for estimating PSF
        images in each tile.
    
    psf_snrtop
        This parameter is a float (default is 0.15). If ``psf_generators`` is 'calibrator', then the peak pixels of Gaussians
        which are the ``psf_snrtop`` fraction of the SNR distribution are taken as Voronoi
        generators. 

    flagchan_rms         
        This parameter is a Boolean (default is ``True``). If ``True``, then the clipped rms and median (r and m) of the clipped rms of
        each channel is calculated. Those channels whose clipped rms is greater
        than 4r away from m are flagged prior to averaging and calculating
        spectral indices from the image cube. However, these channels are
        flagged only if the total number of these bad channels does not exceed
        10% of the total number of channels themselves.                 
                         
    specind_maxchan
        This parameter is an integer (default is 0) that sets the maximum number of channels that can be averaged together to attempt to reach the target SNR set by the ``specind_snr`` parameter. If 0, there is no limit to the number of channels that can be averaged. If 1, no averaging will be done.
    
    specind_snr
        This parameter is a float (default is 3.0) that sets the target SNR to use when fitting for the spectral index. If there is insufficient SNR, neighboring channels are averaged to obtain the target SNR. The maximum allowable number of channels to average is determined by the ``specind_maxchan`` parameter. Channels (after averaging) that fail to meet the target SNR are not used in fitting.
 
    shapelet_basis
        This parameter is a string (default is ``'cartesian'``) that determines the type of shapelet
        basis used. Currently however, only cartesian is supported.
  
    shapelet_fitmode
        This parameter is a string (default is ``'fit'``) that determines the method of calculating
        shapelet coefficients. If ``None``, then these are calculated by integrating
        (actually, by summing over pixels, which introduces errors due to
        discretisation). If 'fit', then the coefficients are found by
        least-squares fitting of the shapelet basis functions to the image.
           
    outfile
        This parameter is a string (default is ``None``) that sets the name of the output file. If ``None``, the file is named automatically.
    
    bbs_patches
        This parameter is a string (default is ``None``) that sets the type of patch to use in BBS-formatted catalogs. When the Gaussian catalogue is written as a BBS-readable sky file, this
        determines whether all Gaussians are in a single patch (``'single'``), there are no
        patches (``None``), all Gaussians for a given source are in a separate patch (``'source'``), or
        each Gaussian gets its own patch (``'gaussian'``).
        
        If you wish to have patches defined by island, then set
        ``group_by_isl = True`` before fitting to force all
        Gaussians in an island to be in a single source. Then set
        ``bbs_patches = 'source'`` when writing the catalog.

    catalog_type
        This parameter is a string (default is ``'gaul'``) that sets the type of catalog to write:  ``'gaul'`` - Gaussian list, ``'srl'`` - source list
        (formed by grouping Gaussians), ``'shap'`` - shapelet list (not yet
        supported)
        
    clobber
        This parameter is a Boolean (default is ``False``) that determines whether existing files are overwritten or not.
        
    format
        This parameter is a string (default is ``'bbs'``) that sets the format of the output catalog. The following formats are supported:

        * ``'bbs'`` - BlackBoard Selfcal sky model format (Gaussian list only)
        
        * ``'ds9'`` - ds9 region format
        
        * ``'fits'`` - FITS catalog format, readable by many software packages, including IDL, TOPCAT, Python, fv, Aladin, etc.
        
        * ``'star'`` - AIPS STAR format (Gaussian list only)
        
        * ``'kvis'`` - kvis format (Gaussian list only)
        
        * ``'ascii'`` - simple text file
        
        Catalogues with the ``'fits'`` and ``'ascii'`` formats include all available
        information (see :ref:`output_cols` for column definitions). The
        other formats include only a subset of the full information.
        
    incl_wavelet
        This parameter is a Boolean (default is ``True``) that determines whether Gaussians fit to wavelet images are included in the output.
        
    srcroot
        This parameter is a string (default is ``None``) that sets the root for source names in the output catalog.
        
    pi_fit    
        This parameter is a Boolean (default is ``True``). If ``True``, the polarized intensity image is searched for sources not
        present in the Stokes I image. If any such sources are found, they are
        added to the the Stokes I source lists. Use the ``pi_thresh_pix`` and
        ``pi_thresh_isl`` parameters to control island detection in the PI image.
    
    pi_thresh_isl
        This parameter is a float (default is ``None``) that determines the region to which fitting is done in the
        polarized intensity (PI) image. If ``None``, the value is set to that of the ``thresh_isl`` parameter. A higher value will produce smaller
        islands, and hence smaller regions that are considered in the fits. A
        lower value will produce larger islands. Use the ``pi_thresh_pix`` parameter
        to set the detection threshold for sources. Generally, ``pi_thresh_isl``
        should be lower than ``pi_thresh_pix``.

    pi_thresh_pix
        This parameter is a float (default is ``None``) that sets the overall detection threshold for islands in the
        polarized intensity (PI) image (i.e. pi_thresh_pix = 5 will find all
        sources with peak fluxes of 5-sigma or greater). If ``None``, the value is set to that of the ``thresh_pix`` parameter. Use the ``pi_thresh_isl``
        parameter to control how much of each island is used in fitting.
        Generally, ``pi_thresh_pix`` should be larger than ``pi_thresh_isl``.

    detection_image
        This parameter is a string (default is ``''``) that sets the detection image file name used only for detecting islands of emission. Source measurement is still done on the main image. The detection image can be a FITS or CASA 2-, 3-, or 4-D cube and must have the same size and WCS parameters as the main image.
