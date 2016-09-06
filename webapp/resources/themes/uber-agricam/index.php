<!DOCTYPE HTML>
<!--
	Multiverse by HTML5 UP
	html5up.net | @ajlkn
	Free for personal and commercial use under the CCA 3.0 license (html5up.net/license)
-->
<html>
	<head>
		<title>GF Agricam</title>
		<meta charset="utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no" />
        
        <!--[if lte IE 8]><script src="<?php echo THEMEPATH; ?>/assets/js/ie/html5shiv.js"></script><![endif]-->
        <link rel="stylesheet" href="<?php echo THEMEPATH; ?>/assets/css/main.css" />
        <!--[if lte IE 9]><link rel="stylesheet" href="<?php echo THEMEPATH; ?>/assets/css/ie9.css" /><![endif]-->
        <!--[if lte IE 8]><link rel="stylesheet" href="<?php echo THEMEPATH; ?>/assets/css/ie8.css" /><![endif]-->
	</head>
	<body>

		<!-- Wrapper -->
			<div id="wrapper">

				<!-- Header -->
					<header id="header">
						<h1><a href="/agricam/"><strong>Agricam</strong> by Grameen Foundation</a></h1>
						<nav>
							<ul>
								<li><a href="#footer" class="icon fa-info-circle">About</a></li>
							</ul>
						</nav>
					</header>

				<!-- Main -->
					<div id="main">
                       <?php if (!empty($galleryArray) && $galleryArray['stats']['total_images'] > 0): ?>
                       <?php foreach ($galleryArray['images'] as $image): ?>
						<article class="thumb">
                            <a href="<?php echo html_entity_decode($image['file_path']); ?>" class="image">
                                <img src="<?php echo html_entity_decode($image['file_path']); ?>" alt="" />
                            </a>
                            <h2><?php echo $image['file_title'] ?></h2>
                            <p>Picture taken at <?php echo $image['file_title_plain']?></p>
						</article>
                       <?php endforeach; ?>
                       <?php endif; ?>
					</div>

				<!-- Footer -->
					<footer id="footer" class="panel">
						<div class="inner split">
							<div>
								<section>
									<h2>Agricam</h2>
                                    <p>The Agriculture Camera (Agricam) project uses a low cost device designed and developed in Grameen Ghana's makerspace
                                        to assist agriculture investors assess risk through remote monitoring of their investments. The camera
                                        solution is solar powered and designed to work in remote locations communicating via a 2G cellular network.</p>
								</section>
								<section>
									<h2>Follow the project on ...</h2>
									<ul class="icons">
										<li><a href="#" class="icon fa-instagram"><span class="label">Instagram</span></a></li>
										<li><a href="#" class="icon fa-github"><span class="label">GitHub</span></a></li>
									</ul>
								</section>
								<p class="copyright">
									&copy; Grameen Foundation. 
								</p>
							</div>
							<div>
								<section>
									<h2>Get in touch</h2>
									<form method="post" action="#">
										<div class="field half first">
											<input type="text" name="name" id="name" placeholder="Name" />
										</div>
										<div class="field half">
											<input type="text" name="email" id="email" placeholder="Email" />
										</div>
										<div class="field">
											<textarea name="message" id="message" rows="4" placeholder="Message"></textarea>
										</div>
										<ul class="actions">
											<li><input type="submit" value="Send" class="special" /></li>
											<li><input type="reset" value="Reset" /></li>
										</ul>
									</form>
								</section>
							</div>
						</div>
					</footer>

			</div>

		    <!-- Scripts -->
            <script src="<?php echo THEMEPATH; ?>/assets/js/jquery.min.js"></script>
            <script src="<?php echo THEMEPATH; ?>/assets/js/jquery.poptrox.min.js"></script>
            <script src="<?php echo THEMEPATH; ?>/assets/js/skel.min.js"></script>
            <script src="<?php echo THEMEPATH; ?>/assets/js/util.js"></script>
            <!--[if lte IE 8]><script src="<?php echo THEMEPATH; ?>/assets/js/ie/respond.min.js"></script><![endif]-->
            <script src="<?php echo THEMEPATH; ?>/assets/js/main.js"></script>
	</body>
</html>
